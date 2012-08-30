{ XPATH=$1; gsub(/\[/,"\\[",XPATH); gsub(/\]/,"\\]",XPATH); cmnd="grep '"XPATH"' "F; system( cmnd ); }
