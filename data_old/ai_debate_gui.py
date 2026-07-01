"""
AI Debate Loop - GUI 버전
Claude vs ChatGPT 토론을 GUI에서 실행
"""

import os
import sys
import json
import threading
import tkinter as tk
from tkinter import ttk, filedialog, scrolledtext, messagebox
from datetime import datetime

try:
    import anthropic
except ImportError:
    print("pip install anthropic")
    sys.exit(1)

try:
    import openai
except ImportError:
    print("pip install openai")
    sys.exit(1)


CLAUDE_SYSTEM_PROMPT = """당신은 비판적 사고를 하는 AI 토론자입니다.
상대방(ChatGPT)의 답변을 검토하고, 오류·부족한 점·논리적 허점을 명확히 지적하세요.
단순 반박이 아닌, 더 정확하고 완전한 답변을 향해 논증을 발전시키세요.
구체적 근거와 함께 의견을 제시하고, 상대방이 옳은 부분은 인정하되 개선점을 제안하세요."""

GPT_SYSTEM_PROMPT = """당신은 비판적 사고를 하는 AI 토론자입니다.
상대방(Claude)의 답변을 검토하고, 오류·부족한 점·논리적 허점을 명확히 지적하세요.
단순 반박이 아닌, 더 정확하고 완전한 답변을 향해 논증을 발전시키세요.
구체적 근거와 함께 의견을 제시하고, 상대방이 옳은 부분은 인정하되 개선점을 제안하세요."""


class DebateGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("AI Debate Loop — Claude vs ChatGPT")
        self.root.geometry("1000x780")
        self.root.configure(bg="#1e1e2e")

        self.attached_files: list[str] = []
        self.is_running = False

        self._build_ui()

    # ──────────────────────────────────────────────
    #  UI 구성
    # ──────────────────────────────────────────────
    def _build_ui(self):
        DARK = "#1e1e2e"
        PANEL = "#2a2a3e"
        ACCENT = "#7c6af7"
        TEXT = "#cdd6f4"
        SUBTEXT = "#a6adc8"
        ENTRY_BG = "#313244"

        style = ttk.Style()
        style.theme_use("clam")
        style.configure("TLabel", background=DARK, foreground=TEXT)
        style.configure("TFrame", background=DARK)
        style.configure("TLabelframe", background=PANEL, foreground=TEXT)
        style.configure("TLabelframe.Label", background=PANEL, foreground=ACCENT, font=("Helvetica", 10, "bold"))
        style.configure("TSpinbox", fieldbackground=ENTRY_BG, foreground=TEXT, background=PANEL)
        style.configure("TCombobox", fieldbackground=ENTRY_BG, foreground=TEXT, background=PANEL)
        style.map("TCombobox", fieldbackground=[("readonly", ENTRY_BG)])

        # ── 상단 타이틀 ──
        title_frame = tk.Frame(self.root, bg=DARK)
        title_frame.pack(fill="x", padx=20, pady=(16, 4))
        tk.Label(title_frame, text="🤖  AI Debate Loop", font=("Helvetica", 18, "bold"),
                 bg=DARK, fg=ACCENT).pack(side="left")
        tk.Label(title_frame, text="Claude vs ChatGPT", font=("Helvetica", 11),
                 bg=DARK, fg=SUBTEXT).pack(side="left", padx=12, pady=4)

        # ── 설정 행 ──
        cfg_frame = ttk.LabelFrame(self.root, text="설정", padding=12)
        cfg_frame.pack(fill="x", padx=20, pady=6)

        # 라운드 수
        tk.Label(cfg_frame, text="토론 라운드 수", bg=PANEL, fg=TEXT,
                 font=("Helvetica", 10)).grid(row=0, column=0, sticky="w", padx=(0, 8))
        self.rounds_var = tk.IntVar(value=3)
        rounds_spin = ttk.Spinbox(cfg_frame, from_=1, to=10, textvariable=self.rounds_var,
                                  width=5, font=("Helvetica", 11))
        rounds_spin.grid(row=0, column=1, sticky="w", padx=(0, 24))

        # Claude 모델
        tk.Label(cfg_frame, text="Claude 모델", bg=PANEL, fg=TEXT,
                 font=("Helvetica", 10)).grid(row=0, column=2, sticky="w", padx=(0, 8))
        self.claude_model_var = tk.StringVar(value="claude-opus-4-8")
        ttk.Combobox(cfg_frame, textvariable=self.claude_model_var, width=22,
                     values=["claude-opus-4-8", "claude-sonnet-4-6", "claude-haiku-4-5-20251001"],
                     state="readonly").grid(row=0, column=3, sticky="w", padx=(0, 24))

        # GPT 모델
        tk.Label(cfg_frame, text="GPT 모델", bg=PANEL, fg=TEXT,
                 font=("Helvetica", 10)).grid(row=0, column=4, sticky="w", padx=(0, 8))
        self.gpt_model_var = tk.StringVar(value="gpt-4o")
        ttk.Combobox(cfg_frame, textvariable=self.gpt_model_var, width=16,
                     values=["gpt-4o", "gpt-4o-mini", "gpt-4-turbo"],
                     state="readonly").grid(row=0, column=5, sticky="w")

        # ── 질문 입력 ──
        q_frame = ttk.LabelFrame(self.root, text="질문 / 프롬프트", padding=12)
        q_frame.pack(fill="x", padx=20, pady=6)

        self.question_text = tk.Text(q_frame, height=4, font=("Helvetica", 11),
                                     bg=ENTRY_BG, fg=TEXT, insertbackground=TEXT,
                                     relief="flat", wrap="word")
        self.question_text.pack(fill="x")
        self.question_text.insert("1.0", "여기에 토론할 질문을 입력하세요...")
        self.question_text.bind("<FocusIn>", self._clear_placeholder)

        # ── 첨부파일 ──
        file_frame = ttk.LabelFrame(self.root, text="첨부파일 (선택)", padding=10)
        file_frame.pack(fill="x", padx=20, pady=6)

        file_btn_row = tk.Frame(file_frame, bg=PANEL)
        file_btn_row.pack(fill="x")

        tk.Button(file_btn_row, text="+ 파일 추가", command=self._add_file,
                  bg=ACCENT, fg="white", relief="flat", font=("Helvetica", 10, "bold"),
                  padx=12, pady=4, cursor="hand2").pack(side="left")
        tk.Button(file_btn_row, text="전체 삭제", command=self._clear_files,
                  bg="#585b70", fg="white", relief="flat", font=("Helvetica", 10),
                  padx=12, pady=4, cursor="hand2").pack(side="left", padx=8)

        self.file_list_var = tk.StringVar(value="첨부된 파일 없음")
        tk.Label(file_btn_row, textvariable=self.file_list_var, bg=PANEL,
                 fg=SUBTEXT, font=("Helvetica", 9)).pack(side="left", padx=12)

        # ── 실행 버튼 ──
        btn_row = tk.Frame(self.root, bg=DARK)
        btn_row.pack(fill="x", padx=20, pady=8)

        self.start_btn = tk.Button(btn_row, text="▶  토론 시작", command=self._start_debate,
                                   bg=ACCENT, fg="white", relief="flat",
                                   font=("Helvetica", 12, "bold"),
                                   padx=20, pady=8, cursor="hand2")
        self.start_btn.pack(side="left")

        tk.Button(btn_row, text="결과 저장 (JSON)", command=self._save_result,
                  bg="#313244", fg=TEXT, relief="flat", font=("Helvetica", 10),
                  padx=14, pady=8, cursor="hand2").pack(side="left", padx=10)

        tk.Button(btn_row, text="초기화", command=self._reset,
                  bg="#313244", fg=TEXT, relief="flat", font=("Helvetica", 10),
                  padx=14, pady=8, cursor="hand2").pack(side="left")

        self.status_var = tk.StringVar(value="대기 중")
        tk.Label(btn_row, textvariable=self.status_var, bg=DARK,
                 fg=SUBTEXT, font=("Helvetica", 10)).pack(side="right", padx=4)

        # ── 출력 영역 ──
        out_frame = ttk.LabelFrame(self.root, text="토론 내용", padding=8)
        out_frame.pack(fill="both", expand=True, padx=20, pady=(0, 16))

        self.output = scrolledtext.ScrolledText(
            out_frame, font=("Courier", 10), bg="#181825", fg=TEXT,
            insertbackground=TEXT, relief="flat", wrap="word", state="disabled"
        )
        self.output.pack(fill="both", expand=True)

        # 태그 색상 설정
        self.output.tag_config("claude", foreground="#89b4fa", font=("Courier", 10, "bold"))
        self.output.tag_config("gpt", foreground="#a6e3a1", font=("Courier", 10, "bold"))
        self.output.tag_config("summary", foreground="#f9e2af", font=("Courier", 10, "bold"))
        self.output.tag_config("system", foreground="#6c7086", font=("Courier", 9, "italic"))
        self.output.tag_config("divider", foreground="#585b70")

        self.debate_log = None

    # ──────────────────────────────────────────────
    #  이벤트 핸들러
    # ──────────────────────────────────────────────
    def _clear_placeholder(self, event):
        if self.question_text.get("1.0", "end-1c") == "여기에 토론할 질문을 입력하세요...":
            self.question_text.delete("1.0", tk.END)

    def _add_file(self):
        paths = filedialog.askopenfilenames(
            title="파일 선택",
            filetypes=[("텍스트/문서", "*.txt *.md *.pdf *.csv *.json *.py"),
                       ("모든 파일", "*.*")]
        )
        for p in paths:
            if p not in self.attached_files:
                self.attached_files.append(p)
        self._refresh_file_label()

    def _clear_files(self):
        self.attached_files.clear()
        self._refresh_file_label()

    def _refresh_file_label(self):
        if self.attached_files:
            names = ", ".join(os.path.basename(p) for p in self.attached_files)
            self.file_list_var.set(f"📎 {names}")
        else:
            self.file_list_var.set("첨부된 파일 없음")

    def _start_debate(self):
        if self.is_running:
            return

        question = self.question_text.get("1.0", "end-1c").strip()
        if not question or question == "여기에 토론할 질문을 입력하세요...":
            messagebox.showwarning("입력 필요", "질문을 입력해주세요.")
            return

        if not os.environ.get("ANTHROPIC_API_KEY"):
            messagebox.showerror("API 키 없음", "ANTHROPIC_API_KEY 환경변수를 설정해주세요.")
            return
        if not os.environ.get("OPENAI_API_KEY"):
            messagebox.showerror("API 키 없음", "OPENAI_API_KEY 환경변수를 설정해주세요.")
            return

        # 첨부파일 내용을 질문에 추가
        full_question = question
        if self.attached_files:
            file_contents = []
            for path in self.attached_files:
                try:
                    with open(path, "r", encoding="utf-8", errors="ignore") as f:
                        content = f.read()
                    file_contents.append(f"[파일: {os.path.basename(path)}]\n{content}")
                except Exception as e:
                    file_contents.append(f"[파일: {os.path.basename(path)}] 읽기 실패: {e}")
            full_question += "\n\n---\n" + "\n\n".join(file_contents)

        self._reset_output()
        self.is_running = True
        self.start_btn.config(state="disabled", text="⏳  토론 진행 중...")

        thread = threading.Thread(
            target=self._run_debate_thread,
            args=(full_question, self.rounds_var.get(),
                  self.claude_model_var.get(), self.gpt_model_var.get()),
            daemon=True
        )
        thread.start()

    def _run_debate_thread(self, question, rounds, claude_model, gpt_model):
        try:
            self._update_status("Claude 초기 답변 생성 중...")
            log = run_debate(
                question=question,
                rounds=rounds,
                claude_model=claude_model,
                gpt_model=gpt_model,
                log_callback=self._append_output,
                status_callback=self._update_status,
            )
            self.debate_log = log
            self._update_status("토론 완료 ✓")
        except Exception as e:
            self._append_output(f"\n오류 발생: {e}\n", "system")
            self._update_status(f"오류: {e}")
        finally:
            self.is_running = False
            self.root.after(0, lambda: self.start_btn.config(
                state="normal", text="▶  토론 시작"))

    def _append_output(self, text, tag=None):
        def _do():
            self.output.config(state="normal")
            if tag:
                self.output.insert(tk.END, text, tag)
            else:
                self.output.insert(tk.END, text)
            self.output.see(tk.END)
            self.output.config(state="disabled")
        self.root.after(0, _do)

    def _update_status(self, msg):
        self.root.after(0, lambda: self.status_var.set(msg))

    def _reset_output(self):
        self.output.config(state="normal")
        self.output.delete("1.0", tk.END)
        self.output.config(state="disabled")

    def _save_result(self):
        if not self.debate_log:
            messagebox.showinfo("저장 불가", "먼저 토론을 실행해주세요.")
            return
        path = filedialog.asksaveasfilename(
            defaultextension=".json",
            filetypes=[("JSON", "*.json")],
            initialfile=f"debate_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        )
        if path:
            with open(path, "w", encoding="utf-8") as f:
                json.dump(self.debate_log, f, ensure_ascii=False, indent=2)
            messagebox.showinfo("저장 완료", f"저장됨:\n{path}")

    def _reset(self):
        self._reset_output()
        self.question_text.delete("1.0", tk.END)
        self.question_text.insert("1.0", "여기에 토론할 질문을 입력하세요...")
        self.attached_files.clear()
        self._refresh_file_label()
        self.debate_log = None
        self._update_status("대기 중")


# ──────────────────────────────────────────────────────────────
#  토론 로직 (GUI 콜백 지원)
# ──────────────────────────────────────────────────────────────

def ask_claude(client, messages, model):
    response = client.messages.create(
        model=model, max_tokens=2048,
        system=CLAUDE_SYSTEM_PROMPT, messages=messages,
    )
    return response.content[0].text


def ask_gpt(client, messages, model):
    response = client.chat.completions.create(
        model=model, max_tokens=2048,
        messages=[{"role": "system", "content": GPT_SYSTEM_PROMPT}] + messages,
    )
    return response.choices[0].message.content


def run_debate(question, rounds, claude_model, gpt_model, log_callback, status_callback):
    claude_client = anthropic.Anthropic(api_key=os.environ["ANTHROPIC_API_KEY"])
    gpt_client = openai.OpenAI(api_key=os.environ["OPENAI_API_KEY"])

    log = {"question": question, "timestamp": datetime.now().isoformat(),
           "claude_model": claude_model, "gpt_model": gpt_model, "rounds": []}

    claude_msgs: list[dict] = []
    gpt_msgs: list[dict] = []

    def divider(title):
        log_callback(f"\n{'─'*60}\n", "divider")
        log_callback(f"  {title}\n", "divider")
        log_callback(f"{'─'*60}\n", "divider")

    # 라운드 1: Claude 초기 답변
    status_callback("Claude 초기 답변 생성 중...")
    divider("Claude — 초기 답변")
    claude_msgs.append({"role": "user", "content": question})
    claude_resp = ask_claude(claude_client, claude_msgs, claude_model)
    claude_msgs.append({"role": "assistant", "content": claude_resp})
    log_callback("[ Claude ]\n", "claude")
    log_callback(claude_resp + "\n")
    log["rounds"].append({"round": 1, "claude": claude_resp, "gpt": None})

    for rnd in range(1, rounds + 1):
        # GPT 반박
        status_callback(f"ChatGPT 반박 중... (라운드 {rnd})")
        divider(f"ChatGPT — 라운드 {rnd} 반박")
        gpt_prompt = (f"[원래 질문]\n{question}\n\n"
                      f"[Claude의 답변 (라운드 {rnd})]\n{claude_resp}\n\n"
                      "위 답변의 문제점을 지적하고 더 정확한 답변을 제시하세요.")
        gpt_msgs.append({"role": "user", "content": gpt_prompt})
        gpt_resp = ask_gpt(gpt_client, gpt_msgs, gpt_model)
        gpt_msgs.append({"role": "assistant", "content": gpt_resp})
        log_callback("[ ChatGPT ]\n", "gpt")
        log_callback(gpt_resp + "\n")
        log["rounds"][-1]["gpt"] = gpt_resp

        if rnd == rounds:
            break

        # Claude 재반박
        status_callback(f"Claude 재반박 중... (라운드 {rnd + 1})")
        divider(f"Claude — 라운드 {rnd + 1} 재반박")
        claude_prompt = (f"[원래 질문]\n{question}\n\n"
                         f"[ChatGPT의 반박 (라운드 {rnd})]\n{gpt_resp}\n\n"
                         "위 반박의 문제점을 지적하고 더 정확한 답변을 제시하세요.")
        claude_msgs.append({"role": "user", "content": claude_prompt})
        claude_resp = ask_claude(claude_client, claude_msgs, claude_model)
        claude_msgs.append({"role": "assistant", "content": claude_resp})
        log_callback("[ Claude ]\n", "claude")
        log_callback(claude_resp + "\n")
        log["rounds"].append({"round": rnd + 1, "claude": claude_resp, "gpt": None})

    # 최종 요약
    status_callback("최종 요약 생성 중...")
    divider("최종 요약 (Claude)")
    history = "\n\n".join(
        f"라운드 {r['round']}\nClaude: {r['claude']}\nChatGPT: {r.get('gpt','없음')}"
        for r in log["rounds"]
    )
    summary_prompt = (f"원래 질문: {question}\n\n토론 내용:\n{history}\n\n"
                      "토론을 바탕으로 최종 정리:\n"
                      "1. 양측 합의 내용\n2. 여전히 논쟁 중인 부분\n3. 가장 정확한 최종 답변")
    claude_msgs.append({"role": "user", "content": summary_prompt})
    summary = ask_claude(claude_client, claude_msgs, claude_model)
    log_callback("[ 최종 요약 ]\n", "summary")
    log_callback(summary + "\n")
    log["final_summary"] = summary

    return log


# ──────────────────────────────────────────────────────────────
if __name__ == "__main__":
    root = tk.Tk()
    app = DebateGUI(root)
    root.mainloop()
