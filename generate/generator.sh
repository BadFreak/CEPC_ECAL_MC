#!/bin/sh
path="$PWD"
cd $path

particle=e-
energy=10
condor_out="$path/out/${particle}/${energy}GeV"
condor_err="$path/err/${particle}/${energy}GeV"
condor_log="$path/log/${particle}/${energy}GeV"
condor_cmd="$path/cmd/${particle}/${energy}GeV"
condor_sh="$path/run/${particle}/${energy}GeV"
condor_mac="$path/mac/${particle}/${energy}GeV"
condor_root="/home/wangjx/HEP/CEPC2023/Result_MC/generate_root/${particle}/${energy}GeV"

mkdir -p $condor_out $condor_err $condor_log $condor_cmd $condor_sh $condor_mac $condor_root
rm -f $condor_out/* $condor_err/* $condor_log/* $condor_cmd/* $condor_sh/* $condor_mac/* $condor_root/*

JobNum=10
n=1
while [ "$n" -le "$JobNum" ]
do
	output_file=${n}
#--------Create mac file------------------------------
	echo "Create ${n} mac file"
	echo \
	"
	/control/verbose 0
	/run/verbose 0
	/event/verbose 0
	
	/mydet/generator fGParticleSource
	/gps/particle ${particle}
	/gps/energy ${energy} GeV
	/gps/direction 0 0 1
		
	# 设置高斯分布
	# /gps/pos/type Beam
	# /gps/pos/shape Ellipse
	# /gps/pos/radius 10 cm
	# /gps/pos/sigma_r 2. cm
	# /gps/pos/centre -0.1846 0.7962  -60. cm
	# /gps/pos/sigma_x 1.2954 cm
	# /gps/pos/sigma_y 0.9838 cm

	# 设置平面源的形状为正方形
	/gps/pos/shape Square
	/gps/pos/centre 0.0 0.0 0.0 cm
	/gps/pos/halfx 10.0 cm
	/gps/pos/halfy 10.0 cm
	# 设置平面法向方向（此例中为沿 Z 轴）
	/gps/pos/rot1 1 0 0
	/gps/pos/rot2 0 1 0
	
	/gps/direction 0 0 1
	/run/beamOn 1000

	" > $condor_mac/event_${output_file}.mac
	
#--------Create .sh file end------------------------------
	echo \
	"#!/bin/sh
		#---- Write your command here------
	
	    ../build/CEPCScECAL $condor_mac/event_${output_file}.mac $condor_root/${output_file}.root
	
		#---- Write your command here------
	
	" > $condor_sh/${output_file}.sh
	
	source ${condor_sh}/${output_file}.sh > ${condor_log}/${output_file}.log &
	let "n+=1"
done

# hadd merge_root electron_10000_40GeV.root  (from) generate_root 

#--------Creat cmd file------------------------------
#	if test -s $condor_cmd/${output_file}.cmd; 
#	then
#		echo "CMD File is not Empty"
#		rm  $condor_cmd/${output_file}.cmd
#	fi
#	cat >> $condor_cmd/${output_file}.cmd << EOF
#	  Universe             = vanilla
#	  Notification         = Never
#	  GetEnv               = True
#	  Executable           = $condor_sh/${output_file}.sh
#	  Output               = $condor_out/${output_file}.out
#	  Error                = $condor_err/${output_file}.err
#	  Log                  = $condor_log/${output_file}.log
#	  Queue								 
#EOF
#	
#	chmod +x $condor_sh/${output_file}.sh
#	chmod +x $condor_cmd/${output_file}.cmd 
#	
#	echo "condor_submit $condor_cmd/${output_file}.cmd"
#	condor_submit $condor_cmd/${output_file}.cmd 
#	
#	let "n+=1"
#done
