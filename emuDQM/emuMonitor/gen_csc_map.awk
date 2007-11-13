BEGIN {printf("var CSCMAP=[\n") }
{printf("[\'crate%d\',\'slot%d\',\'%s\'],\n",$12,1,$3)}
{printf("[\'crate%d\',\'slot%d\',\'%s\'],\n",$12,2,$4)}
{printf("[\'crate%d\',\'slot%d\',\'%s\'],\n",$12,3,$5)}
{printf("[\'crate%d\',\'slot%d\',\'%s\'],\n",$12,4,$6)}
{printf("[\'crate%d\',\'slot%d\',\'%s\'],\n",$12,5,$7)}
{printf("[\'crate%d\',\'slot%d\',\'%s\'],\n",$12,7,$8)}
{printf("[\'crate%d\',\'slot%d\',\'%s\'],\n",$12,8,$9)}
{printf("[\'crate%d\',\'slot%d\',\'%s\'],\n",$12,9,$10)}
{printf("[\'crate%d\',\'slot%d\',\'%s\'],\n",$12,10,$11)}
END {printf("]\n")}
