#! /bin/bash

head -n 1 account_info.csv > account_info_fixed.csv
tail -n +2 account_info.csv | awk -F',' 'BEGIN {OFS=FS}
{$7=(length($7) < 5)?0$7:$7}
{print}' >> account_info_fixed.csv

head -n 1 transactions.csv > transactions_fixed.csv
tail -n +2 transactions.csv | awk -F',' 'BEGIN {OFS=FS}
{split($2,d," ")}
{$2=substr(d[1],0,2)"-"substr(d[1],3,2)"-"substr(d[1],5,4)" "d[2]}
{gsub(/[-+]$/,"",$3)}
{$4=substr($4,0,2)"-"substr($4,3,2)"-"substr($4,5,4)}
{gsub(/US$/,"",$6)}
{print}' >> transactions_fixed.csv
