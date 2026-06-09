"""
AI Debate Loop: Claude vs ChatGPT
동일한 질문에 대해 Claude와 ChatGPT가 서로 반박하며 토론하는 코드.
"""

import os
import sys
import json
import time
from datetime import datetime
from typing import Optional

try:
    import anthropic
except ImportError:
    print("anthropic 패키지 설치 필요: pip install anthropic")
    sys.exit(1)

try:
    import openai
except ImportError:
    print("openai 패키지 설치 필요: pip install openai")
    sys.exit(1)


CLAUDE_SYSTEM_PROMPT = """당신은 비판적 사고를 하는 AI 토론자입니다.
상대방(ChatGPT)의 답변을 검토하고, 오류·부족한 점·논리적 허점을 명확히 지적하세요.
단순 반박이 아닌, 더 정확하고 완전한 답변을 향해 논증을 발전시키세요.
구체적 근거와 함께 의견을 제시하고, 상대방이 옳은 부분은 인정하되 개선점을 제안하세요."""

GPT_SYSTEM_PROMPT = """당신은 비판적 사고를 하는 AI 토론자입니다.
상대방(Claude)의 답변을 검토하고, 오류·부족한 점·논리적 허점을 명확히 지적하세요.
단순 반박이 아닌, 더 정확하고 완전한 답변을 향해 논증을 발전시키세요.
구체적 근거와 함께 의견을 제시하고, 상대방이 옳은 부분은 인정하되 개선점을 제안하세요."""

SUMMARY_PROMPT = """지금까지의 토론을 바탕으로 최종 정리를 해주세요:
1. 양측이 합의한 핵심 내용
2. 여전히 논쟁 중인 부분
3. 가장 정확하고 완전한 최종 답변

토론에서 도달한 가장 올바른 결론을 명확히 제시해주세요."""


def ask_claude(
    client: anthropic.Anthropic,
    messages: list[dict],
    model: str = "claude-opus-4-8",
) -> str:
    response = client.messages.create(
        model=model,
        max_tokens=2048,
        system=CLAUDE_SYSTEM_PROMPT,
        messages=messages,
    )
    return response.content[0].text


def ask_gpt(
    client: openai.OpenAI,
    messages: list[dict],
    model: str = "gpt-4o",
) -> str:
    response = client.chat.completions.create(
        model=model,
        max_tokens=2048,
        messages=[{"role": "system", "content": GPT_SYSTEM_PROMPT}] + messages,
    )
    return response.choices[0].message.content


def format_debate_prompt_for_opponent(
    original_question: str,
    opponent_name: str,
    opponent_response: str,
    round_num: int,
) -> str:
    return f"""[원래 질문]
{original_question}

[{opponent_name}의 답변 (라운드 {round_num})]
{opponent_response}

위 답변에서 문제점이나 보완할 점을 지적하고, 더 정확한 답변을 제시해주세요."""


def run_debate(
    question: str,
    rounds: int = 3,
    claude_model: str = "claude-opus-4-8",
    gpt_model: str = "gpt-4o",
    save_to_file: bool = True,
) -> dict:
    anthropic_key = os.environ.get("ANTHROPIC_API_KEY")
    openai_key = os.environ.get("OPENAI_API_KEY")

    if not anthropic_key:
        raise ValueError("ANTHROPIC_API_KEY 환경변수가 설정되지 않았습니다.")
    if not openai_key:
        raise ValueError("OPENAI_API_KEY 환경변수가 설정되지 않았습니다.")

    claude_client = anthropic.Anthropic(api_key=anthropic_key)
    gpt_client = openai.OpenAI(api_key=openai_key)

    debate_log = {
        "question": question,
        "timestamp": datetime.now().isoformat(),
        "claude_model": claude_model,
        "gpt_model": gpt_model,
        "rounds": [],
    }

    claude_messages: list[dict] = []
    gpt_messages: list[dict] = []

    print("=" * 70)
    print(f"AI 토론 시작: {rounds}라운드")
    print(f"질문: {question}")
    print("=" * 70)

    # 라운드 1: Claude가 먼저 답변
    print(f"\n[라운드 1] Claude 초기 답변 생성 중...")
    claude_messages.append({"role": "user", "content": question})
    claude_response = ask_claude(claude_client, claude_messages, claude_model)
    claude_messages.append({"role": "assistant", "content": claude_response})

    print(f"\n{'='*30} Claude (라운드 1) {'='*30}")
    print(claude_response)

    debate_log["rounds"].append({
        "round": 1,
        "claude": claude_response,
        "gpt": None,
    })

    # 토론 루프
    for round_num in range(1, rounds + 1):
        # GPT가 Claude에 반박
        print(f"\n[라운드 {round_num}] ChatGPT 반박 생성 중...")
        gpt_prompt = format_debate_prompt_for_opponent(
            question, "Claude", claude_response, round_num
        )
        gpt_messages.append({"role": "user", "content": gpt_prompt})
        gpt_response = ask_gpt(gpt_client, gpt_messages, gpt_model)
        gpt_messages.append({"role": "assistant", "content": gpt_response})

        print(f"\n{'='*30} ChatGPT (라운드 {round_num}) {'='*30}")
        print(gpt_response)

        debate_log["rounds"][-1]["gpt"] = gpt_response

        if round_num == rounds:
            break

        # Claude가 GPT에 재반박
        print(f"\n[라운드 {round_num + 1}] Claude 재반박 생성 중...")
        claude_prompt = format_debate_prompt_for_opponent(
            question, "ChatGPT", gpt_response, round_num
        )
        claude_messages.append({"role": "user", "content": claude_prompt})
        claude_response = ask_claude(claude_client, claude_messages, claude_model)
        claude_messages.append({"role": "assistant", "content": claude_response})

        print(f"\n{'='*30} Claude (라운드 {round_num + 1}) {'='*30}")
        print(claude_response)

        debate_log["rounds"].append({
            "round": round_num + 1,
            "claude": claude_response,
            "gpt": None,
        })

        time.sleep(0.5)  # API 레이트 리밋 방지

    # 최종 요약: Claude가 토론 전체를 정리
    print(f"\n{'='*30} 최종 요약 생성 중... {'='*30}")
    debate_history = "\n\n".join(
        f"[라운드 {r['round']}]\nClaude: {r['claude']}\nChatGPT: {r.get('gpt', '없음')}"
        for r in debate_log["rounds"]
    )
    summary_prompt = f"원래 질문: {question}\n\n토론 내용:\n{debate_history}\n\n{SUMMARY_PROMPT}"
    claude_messages.append({"role": "user", "content": summary_prompt})
    final_summary = ask_claude(claude_client, claude_messages, claude_model)

    print(f"\n{'='*30} 최종 요약 {'='*30}")
    print(final_summary)
    debate_log["final_summary"] = final_summary

    if save_to_file:
        filename = f"debate_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        with open(filename, "w", encoding="utf-8") as f:
            json.dump(debate_log, f, ensure_ascii=False, indent=2)
        print(f"\n토론 결과 저장됨: {filename}")

    return debate_log


def main():
    import argparse

    parser = argparse.ArgumentParser(description="Claude vs ChatGPT AI 토론 루프")
    parser.add_argument("question", nargs="?", help="토론할 질문")
    parser.add_argument("--rounds", type=int, default=3, help="토론 라운드 수 (기본값: 3)")
    parser.add_argument("--claude-model", default="claude-opus-4-8", help="Claude 모델")
    parser.add_argument("--gpt-model", default="gpt-4o", help="GPT 모델")
    parser.add_argument("--no-save", action="store_true", help="파일 저장 안 함")
    args = parser.parse_args()

    if args.question:
        question = args.question
    else:
        print("토론할 질문을 입력하세요:")
        question = input("> ").strip()
        if not question:
            print("질문이 없습니다. 종료합니다.")
            sys.exit(1)

    run_debate(
        question=question,
        rounds=args.rounds,
        claude_model=args.claude_model,
        gpt_model=args.gpt_model,
        save_to_file=not args.no_save,
    )


if __name__ == "__main__":
    main()
