#!/bin/bash

totalIO=0
concurrentIO=0
smallIO=0
smallRandIO=0

cat $1 | {
while read line
do
	#echo $line
	IFS=' | ' read -r -a array <<< "$line"
	#echo "${array[0]}"
	#echo "${array[1]}"
	#echo "${array[2]}"
	#echo "${array[3]}"
	#echo "${array[4]}"
	#echo "${array[5]}"

	if [[ ${array[0]} == /users/ingerido/ssd/rocksdb/DATA/dbbench/*.log ]]; then
		let "concurrentIO=concurrentIO+1"
		if [[ ${array[4]} -lt 10240 ]]; then
			let "smallRandIO=smallRandIO+1"
		fi
	fi

	if [[ ${array[0]} == /users/ingerido/ssd/rocksdb/DATA/dbbench/*.sst ]] && [[ ${array[3]} == pread ]]; then
		let "concurrentIO=concurrentIO+1"
		if [[ ${array[4]} -lt 10240 ]]; then
			let "smallRandIO=smallRandIO+1"
		fi
	fi

	if [[ ${array[4]} -lt 10240 ]]; then
		let "smallIO=smallIO+1"
	fi	

	let "totalIO=totalIO+1"
done

echo "totalIO = $totalIO"
echo "concurentIO = $concurrentIO"
echo "smallIO = $smallIO"
echo "smallRandIO = $smallRandIO"

echo "$concurrentIO/$totalIO" | bc -l
echo "$smallIO/$totalIO" | bc -l
echo "$smallRandIO/$totalIO" | bc -l

}
