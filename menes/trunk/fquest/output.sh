#!/bin/bash
user=$1
pass=$2
shift 2

wget --post-data="reason=&Access_ID=$user&Current_Passcode=$pass&acct=&pswd=&from=homepage&Customer_Type=CA&id=$user&rembme=Y&pc=$pass&state=CA&olb_signin=Sign%20In" -O - -U "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; .NET CLR 1.0.3705; .NET CLR 1.1.4322)" "https://onlineid.bankofamerica.com/cgi-bin/sso.login.controller" | ../utils/tidy.sh - | ../utils/saxon.sh - output.xsl | grep -vFf output.dat | tee -a output.dat | tac
