#!/bin/sh
set -eu
TT4="5"
TT5="3"
ST="7 4"
SST="7 0 0 3"
SSST="5888889"
BIN=epoll_server

txt()
{
	echo $(cat $1)
}

res()
{
	if [ "$2" = "$(txt $3)" ]
	then
		echo "+ Good, $1 test passed"
	else
		echo "- Bad, $1 test failed."
		echo "    I do expect: '$2'"
		echo "    I found: '$(txt $3)'"
		ex=1
	fi
}

test -x $BIN && run=$BIN
test -x builddir/$BIN && run=builddir/$BIN
if [ -z "$run" ]
then
	echo "No $BIN binary found."
	exit 1
fi

tr=$(mktemp)
tr4=$(mktemp)
tr5=$(mktemp)
sr=$(mktemp)
ssr=$(mktemp)
sssr=$(mktemp)

(
	( sleep 2; echo -n "foo"; sleep 2; echo -n " bar\nbuzz\n"; ) \
	| ( sleep 0.1; nc -q 1 localhost 12345 >$sr; )
)&
(
	(
		sleep 2; echo -n "this"; \
		sleep 1; echo -n " is\n"; \
		sleep 1; echo -n "\n\nesw\n"; \
		) \
	| ( sleep 0.1; nc -q 1 localhost 12345 >$ssr; )
)&
(
	echo "$(seq 999999|tr -d '\n')" \
	| ( sleep 0.1 ; nc localhost 12345 >$sssr )
)&
timeout 6 ./$run | tee $tr

sed -i '/^[^(timer)].*/d' $tr
cat $tr | grep "timer: 4" | wc -l > $tr4
cat $tr | grep "timer: 5" | wc -l > $tr5

ex=0
res "timer 4" "$TT4" "$tr4"
res "timer 5" "$TT5" "$tr5"
res "1st socket" "$ST" "$sr"
res "2nd socket" "$SST" "$ssr"
res "3rd socket" "$SSST" "$sssr"

rm $tr $sr $ssr $sssr
exit $ex
