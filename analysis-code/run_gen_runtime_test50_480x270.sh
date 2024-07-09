python gen_runtime_by_batch.py -o "runtime-icdar-resolutions/480x270/test50/runtime_0.csv" -i "../dataset-icdar/480x270/test50" -e rhn
sleep 5
python gen_runtime_by_batch.py -o "runtime-icdar-resolutions/480x270/test50/runtime_1.csv" -i "../dataset-icdar/480x270/test50" -e rnh
sleep 5
python gen_runtime_by_batchl.py -o "runtime-icdar-resolutions/480x270/test50/runtime_2.csv" -i "../dataset-icdar/480x270/test50" -e nhr
sleep 5
python gen_runtime_by_batch.py -o "runtime-icdar-resolutions/480x270/test50/runtime_3.csv" -i "../dataset-icdar/480x270/test50" -e nrh
sleep 5
python gen_runtime_by_batch.py -o "runtime-icdar-resolutions/480x270/test50/runtime_4.csv" -i "../dataset-icdar/480x270/test50" -e hnr
sleep 5
python gen_runtime_by_batch.py -o "runtime-icdar-resolutions/480x270/test50/runtime_5.csv" -i "../dataset-icdar/480x270/test50" -e hrn
sleep 5
python gen_runtime_by_batch.py -o "runtime-icdar-resolutions/480x270/test50/runtime_6.csv" -i "../dataset-icdar/480x270/test50" -e rhn
sleep 5
python gen_runtime_by_batch.py -o "runtime-icdar-resolutions/480x270/test50/runtime_7.csv" -i "../dataset-icdar/480x270/test50" -e rnh
sleep 5
python gen_runtime_by_batch.py -o "runtime-icdar-resolutions/480x270/test50/runtime_8.csv" -i "../dataset-icdar/480x270/test50" -e nhr
sleep 5
python gen_runtime_by_batch.py -o "runtime-icdar-resolutions/480x270/test50/runtime_9.csv" -i "../dataset-icdar/480x270/test50" -e nrh
sleep 5
python gen_runtime_by_batch.py -o "runtime-icdar-resolutions/480x270/test50/runtime_10.csv" -i "../dataset-icdar/480x270/test50" -e hnr
sleep 5
python gen_runtime_by_batch.py -o "runtime-icdar-resolutions/480x270/test50/runtime_11.csv" -i "../dataset-icdar/480x270/test50" -e hrn
sleep 5
echo "DONE"