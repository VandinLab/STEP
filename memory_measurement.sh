pid=$1
SECONDS=0
fname="memory_consumption.txt"
echo "" > ${fname}
trap "kill $pid 2> /dev/null" EXIT
echo $pid
echo "ANALYSIS STARTING" >> $fname
while kill -0 ${pid} 2> /dev/null
do
      duration=$SECONDS
      echo -n "${duration} " >> $fname & ps -o rss= $pid | awk '{printf "%.0f\n", $1 / 1024}' >> $fname
      sleep 1.5
done
echo "ANALYSIS ENDING">> $fname
trap - EXIT