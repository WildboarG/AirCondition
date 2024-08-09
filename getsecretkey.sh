#!/bin/bash

url="https://pro.bemfa.com/vs/web/v1/userSecretKey"

read -p "please input your bafa openID:" myid
#echo $myid
data=$(curl -X POST $url \
    -H "Content-Type: application/json" \
    -H "charset: utf-8" \
    -d '{"openID":"'$myid'"}'\
    )
#echo $data
appID=$(echo "$data" | grep -oP '"appID":"\K[^"]+')
secretKey=$(echo "$data" | grep -oP '"secretKey":"\K[^"]+')

echo "appID: $appID"
echo "secretKey: $secretKey"
