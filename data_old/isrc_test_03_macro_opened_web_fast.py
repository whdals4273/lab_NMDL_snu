from selenium.webdriver.common.by import By
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.keys import Keys
 
from selenium.webdriver.support.ui import Select
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
import time

#pip install selenium 터미널에서 설치 후 진행 (pip 안되면 파이선 윈도우 path 경로 체크박스 선택 후 설치)

##시작전 윈도우키+r 누르고 아래 명령어 실행(#빼고 둘중하나되는걸로)
#C:\Program Files (x86)\Google\Chrome\Application\chrome.exe --remote-debugging-port=9222 --user-data-dir="C:/ChromeTEMP"
#C:\Program Files\Google\Chrome\Application\chrome.exe --remote-debugging-port=9222 --user-data-dir="C:/ChromeTEMP"


#반공연홈페이지 접속 후 팝업창 하루 안보기, 로그인 완료 후 프로그램실행.

chrome_options = Options()
chrome_options.add_experimental_option("debuggerAddress", "127.0.0.1:9222")
driver = webdriver.Chrome(options=chrome_options)

driver.get(url='https://isrc.snu.ac.kr/hm/event/reservation/equip?EVENT_SKEY=1480&ESCHED_SKEY=5304&EVENT_SUB_TYPE=RG&FLAG=GOTO&USER_SKEY=&PLAN_AMOUNT=0&STATUS=RE')

time.sleep(0.05)    

#계정코드 검색 및 선택
driver.find_element(By.XPATH,'/html/body/div/div[3]/div/div/form/div[1]/ul[3]/li[2]/a').click()
driver.find_element(By.XPATH,'/html/body/div[2]/div[2]/div/div[3]/div/div[2]/div[1]/div[2]/div[2]/table/tbody/tr').click()
driver.find_element(By.XPATH,'/html/body/div[2]/div[3]/a[1]').click()
driver.find_element(By.XPATH,'/html/body/div/div[3]/div/div/form/div[2]/a[2]').click()

time.sleep(0.1)
driver.find_element(By.XPATH,'/html/body/div[2]/div[2]/div[4]/a').click()