#!/bin/sh
RETVAL=0

input_hpm=image.hpm
output_hpm=image_out.hpm

if [ "$1" != "" ]; then	
	input_hpm=$1
fi

if [ "$2" != "" ]; then	
	output_hpm=$2
fi

#get picmg cappabilities
ipmicmd -k "0f 00 2c 2e 00" smi 0 > picmgcmd.txt
if [ $? != 0 ] ; then
	echo "get picmg support failed"
	exit 1
fi

RETVAL=`cat picmgcmd.txt|awk '{print $5}'`
if [ "$RETVAL" == "c1" ] ; then
	#c1说明已经是去掉了picmg相关命令的版本 走oem升级 升级包无需处理
	echo "bmc not support picmg upgrade type"
    cp $input_hpm $output_hpm
	exit 0
fi
#找到PICMGFWU
offset=$(strings -tx $input_hpm |grep PICMGFWU|head -1)
#预防PICMGFWU前面有空格
match=$(echo "$offset"|sed 's/^ *[0-9a-f]\+ //;s/PICMGFWU.*//')
base=$(echo $offset|awk '{print $1}')
base_num=$((0x$base))
#没有PICMGFWU,没有头则退出
if [ $base_num -lt 1 ] ; then
	exit 0
fi

dd if=$input_hpm of=temp1.hpm ibs=1 obs=512K skip=$((0x$base + 0x${#match}))
#追加md5
echo -ne $(md5sum temp1.hpm |awk '{print $1}'|sed 's/../\\x&/g') >>temp1.hpm
#追加签名
cat>pme_host_rsa<<EOF
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAwQTSkP+wPlDQUsuZD6l995+ThyZ3nbBGCvDYM3AIdWWDMkVQ
X5MUx3QycWeyxiaHThvDASuzIujpyGpymJax1S1Km9zpxJK+RH0JpPTGB6BXNMkv
BbUb7QYgp0EZBuiErtAG4tbPCvUILQ3+2LoBdGAyabZkykPdVSsa/LfjioyKG0YY
fMDPFm93TM4Sfb+ThxkvcAeNaQ1n5/mook2e5cli2DelQDvTjsEtdixdZ8o/XbVn
UbYOG/KuLB1p6nYS7nTHDSRXag8wjw10vjHRkXClF3R7fiSOvAV2AGRK0W/MLGe3
DFFwLAa57KfPkdhDs2AxA2iZ1tHiHdpmwC3zNQIDAQABAoIBAFsD4iKbXsjTpJgW
BzO6cXqtVYx6GBdUQoLwNJJ0tphc6LNwuciHe1ndsdeF1WfYTR8Iujg4Gf1RJMPr
ZeQL0wk5lR/P2K9nnq5+42hUP3jjI5vxbencD7IBnK4hZSK7y6z+zPBz6Cc7icJt
AgnCE+ODZOIyLm6t95UsgUo/j90JOqixmVlNbvm19h29TcjzxJgVQQ8H9FqtxgE8
qfk43LR0krg7iP86wd1c67V3XTa6A2FuQA/e3JNlQ4w2YtlRwDfQfad1BFg96Xaa
tHjw0FFlP1yhgGxvNaOc3R2K385SmImO53cjK5USZbF5BQj2q4QxJirnZZBhxMgE
H0TieUECgYEA/gLQZsWTRFIoFFti1klkNE/bKQGwkacNi5E+Jfjs0vypfldTFInl
m+SRJcIoaCbijkPbnYlQkLmaPaYsfO35qPP3Q4hzM+iyJuh8YqA2fnAqzy3PNW3r
lrNaS6aaGxo6c01j5Xe6NMqqk4Ml/Hx/uwJJTHLDYVIM/0B84KKMbUUCgYEAwoe+
omQXYU3eOyQWqvvwOwp112ij6eJOUiluGmRdl1q6lhbBnP22sSPhkmYUQVl3FjHv
cl6C3HDV4mhRcSKFZcx1Zlvv/a2yey2rNuJmpEDYvuz11+DsjGsHSzwbvUVRG6J4
zg80fTpK+JPyx+AS76vOmHOKAQaq4aP1Rp5y9TECgYA60toCQtUE6GXBuRwddeeP
ucKOL4b9ZAtd++fBkXD+t2ZlDMH/qqOSw47diKYk4IRs38gXJnqS2XifuEDrOY0s
lDr517qeFdBT9FkB2gpko4M2cWxf+jGpt6U8ibt8dUl99DkjJePoX9q6yI9edlJ6
M2BYJOecPQ0ne36v4tTUZQKBgCe3FxCIZs2S3EAuygH2lcE4DX8A+hYSAfN8SeJR
g1oXR+jElJq8ocPgk3kUJiaBYXLClyQ+b3EaCFDFA3uZlKqH56ASPRkJkVrcpAv8
hQAbzSSNtFNytFimsUKG7xLhSAHgVdGcARdgA1ZkvjhYYwVTikZNsGstNU4D71uP
leDBAoGBAMjQR7GMzwrMS2aJkngwS1RoQd+OnBR983NUToj82Ba06cSh54O7bp9q
QHD1xn3eVP20gfpP/x4WfkWmrsG4OxZAJGH2omq2bBHAMhke4GsSq1NCGUI/QJ/T
lbuQCzZ8YJ2I9jJ6VRywDf9oJqc1lFGFx2iIIFBteyb5Y2f5Mihd
-----END RSA PRIVATE KEY-----
EOF


SIGN_FILE=$(echo $$)
openssl dgst -sign pme_host_rsa -sha256 -out $SIGN_FILE temp1.hpm
cat temp1.hpm > $output_hpm
cat $SIGN_FILE >> $output_hpm

rm temp1.hpm
rm $SIGN_FILE
rm pme_host_rsa
echo "modify hpm for bt ok"
