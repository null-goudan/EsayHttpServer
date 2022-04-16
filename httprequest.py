import requests

url = "http://192.168.214.128/"
res = requests.get(url)
print(res.text)