python rescale_batch.py -i 1920x1080/test -o 960x540/test -s 42 -n 50 -r 0.5 
python rescale_batch.py -i 960x540/test -o 480x270/test -s 42 -n 50 -r 0.5 
python rescale_batch.py -i 480x270/test -o 240x135/test -s 42 -n 50 -r 0.5 
python rescale_batch.py -i 240x135/test -o 120x68/test -s 42 -n 50 -r 0.5 

python rescale_batch.py -i 1920x1080/validation -o 960x540/validation -r 0.5 
python rescale_batch.py -i 960x540/validation -o 480x270/validation  -r 0.5 
python rescale_batch.py -i 480x270/validation -o 240x135/validation  -r 0.5 
python rescale_batch.py -i 240x135/validation -o 120x68/validation   -r 0.5 