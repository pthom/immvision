for p in $(ps x | grep python | grep -v killall | grep -v grep | cut -d' ' -f 1); do echo $p; kill -9 $p;  done
