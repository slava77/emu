#!/bin/zsh
# Compares the attribute values of two XML files given as arguments

# Set WITHCOLORS to "n" to disable colored output
WITHCOLORS="y"

TMPDIR=tmp_compareXMLs
E1=$TMPDIR/elements_1
E2=$TMPDIR/elements_2
A1=$TMPDIR/attributes_1
A2=$TMPDIR/attributes_2
A12=$TMPDIR/attributes_1_2
AV1=$TMPDIR/attributeValues_1
AV2=$TMPDIR/attributeValues_2
VCA1=$TMPDIR/valuesOfCommonAttr_1
VCA2=$TMPDIR/valuesOfCommonAttr_2
DVCA1=$TMPDIR/differingValueCommonAttr_1
DVCA2=$TMPDIR/differingValueCommonAttr_2

mkdir -p $TMPDIR

xsltproc elementLister.xsl        $1 | sort > $E1
xsltproc elementLister.xsl        $2 | sort > $E2
xsltproc attributeLister.xsl      $1 | sort > $A1
xsltproc attributeLister.xsl      $2 | sort > $A2
xsltproc attributeValueLister.xsl $1 | sort > $AV1
xsltproc attributeValueLister.xsl $2 | sort > $AV2

print "********************"
print "* Elements only in $1"
print "********************"
comm -23 $E1 $E2

print "********************"
print "* Elements only in $2"
print "********************"
comm -13 $E1 $E2

print "********************"
print "* Attributes only in $1"
print "********************"
comm -23 $A1 $A2 | awk -v F="$AV1" -f compareXMLs.awk

print "********************"
print "* Attributes only in $2"
print "********************"
comm -13 $A1 $A2 | awk -v F="$AV2" -f compareXMLs.awk

autoload -U colors && colors
print "****************************************************"
if [[ $WITHCOLORS == "y" ]] then;
    print "* Differences between values of attributes common to ${fg_bold[red]}$1${reset_color} and ${fg_bold[blue]}$2$reset_color" 
else
    print "* Differences between values of attributes common to $1 and $2"
fi
print "****************************************************"
# Common attributes
comm -12 $A1 $A2 > $A12
# Values in each file of their common attributes
awk -v F="$AV1" -f compareXMLs.awk $A12 > $VCA1
awk -v F="$AV2" -f compareXMLs.awk $A12 > $VCA2
# Common attributes with differing values
comm -23 $VCA1 $VCA2 > $DVCA1
comm -13 $VCA1 $VCA2 > $DVCA2

# diff $VCA1 $VCA2
# Present differences in a clearer way

LINES1=()
LINES2=()
while read LINE; do
    LINES1+=$LINE
done < $DVCA1
while read LINE; do
    LINES2+=$LINE
done < $DVCA2

if [[ ${#LINES1} -eq ${#LINES2} ]]; then
    ((N=${#LINES1}))
else
    print "$DVCA1 and $DVCA2 contain different number of lines. This shouldn't happen. Exiting."
    exit 1
fi

for ((I=1;I<=$N;I++)); do
    # Use 'read' to make sure everything after the XPath expression goes into a single variable (in case the attribute value contains blanks) 
    print ${LINES1[$I]} | read XPATH VALUE1
    print ${LINES2[$I]} | read XPATH VALUE2
    # Remove quotation marks
    VALUE1=${(Q)VALUE1}
    VALUE2=${(Q)VALUE2}
    if [[ $WITHCOLORS == "y" ]] then;
	COLOREDVALUE1=""
	COLOREDVALUE2=""
	[[ ${#VALUE1} -gt ${#VALUE2} ]] && { ((MIN=${#VALUE2})); ((MAX=${#VALUE1}))} || { ((MIN=${#VALUE1})); ((MAX=${#VALUE2})) }
	for ((J=1;J<=$MAX;J++)); do
	    if [[ ${VALUE1[$J]} == ${VALUE2[$J]} ]]; then
		[[ $J -le ${#VALUE1} ]] && COLOREDVALUE1=$COLOREDVALUE1$reset_color${VALUE1[$J]}
		[[ $J -le ${#VALUE2} ]] && COLOREDVALUE2=$COLOREDVALUE2$reset_color${VALUE2[$J]}
	    else
		[[ $J -le ${#VALUE1} ]] && { [[ ${VALUE1[$J]} == '[[:blank:]]' ]] && { COLOREDVALUE1=$COLOREDVALUE1$bg[red]${VALUE1[$J]} } || COLOREDVALUE1=$COLOREDVALUE1$fg_bold[red]${VALUE1[$J]} }
		[[ $J -le ${#VALUE2} ]] && { [[ ${VALUE2[$J]} == '[[:blank:]]' ]] && { COLOREDVALUE2=$COLOREDVALUE2$bg[blue]${VALUE2[$J]} } || COLOREDVALUE2=$COLOREDVALUE2$fg_bold[blue]${VALUE2[$J]} }
	    fi
	done
	print $XPATH
	print $COLOREDVALUE1$reset_color
	print $COLOREDVALUE2$reset_color
    else
	#print $XPATH
	#print $VALUE1
	#print $VALUE2
	print "<set xpath=\"/$XPATH\"       value=\"$VALUE2\"/>"

    fi	
done

rm -rf $TMPDIR
