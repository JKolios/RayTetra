import graph;
import unicode;

	//Actual graph data	
	real cpu_segura0_times[];
	real cpu_stp0_times[];	
	real cpu_stp1_times[];
	real cpu_stp2_times[];

	real gpu_segura0_write_times[];
	real gpu_segura0_exec_times[];
	real gpu_segura0_read_times[];

	real gpu_stp0_write_times[];
	real gpu_stp0_exec_times[];
	real gpu_stp0_read_times[];

	real gpu_stp1_write_times[];
	real gpu_stp1_exec_times[];
	real gpu_stp1_read_times[];

	real gpu_stp2_write_times[];
	real gpu_stp2_exec_times[];
	real gpu_stp2_read_times[];

	//Intersection percentages
	real percentages[];
	//Benchmark parameters
	int total_pairs,repetitions,granularity;

	//Input/output filename
	string filename;

real[] time_per_test(real[] array, int total_pairs, int repetitions)
{


	real[] array_out;
	for(real k:array) array_out.push(k/(total_pairs + repetitions));
	return array_out;
}

void get_input()
{
	file pipe_input = stdin;
	filename = pipe_input + ".csv";
	file in=(input(filename,true,"#"));
			
	real read_val;

	//Calculates the number of input lines
	total_pairs =  in.csv();
 	repetitions = in.csv();
	granularity = in.csv();
	real point_count = (100 / granularity) + 1;
	write("Point Count:",point_count);
	for(int i=0;i<point_count;++i) percentages.push(i * granularity);
		
	
	for(int i=0;i<point_count;++i){

		for(int j=0;j<5;++j) read_val = in.csv();
		read_val = in.csv();
		cpu_segura0_times.push(read_val);
		for(int j=0;j<2;++j) read_val = in.csv();		
		read_val = in.csv();
		cpu_stp0_times.push(read_val);		
		read_val = in.csv();
		cpu_stp1_times.push(read_val);				 
		read_val = in.csv();
		cpu_stp2_times.push(read_val);
		read_val = in.csv();
		gpu_segura0_write_times.push(read_val);
		read_val = in.csv();
		gpu_segura0_exec_times.push(read_val);
		read_val = in.csv();
		gpu_segura0_read_times.push(read_val);
		read_val = in.csv();
		gpu_stp0_write_times.push(read_val);
		read_val = in.csv();
		gpu_stp0_exec_times.push(read_val);
		read_val = in.csv();
		gpu_stp0_read_times.push(read_val);
		read_val = in.csv();
		gpu_stp1_write_times.push(read_val);
		read_val = in.csv();
		gpu_stp1_exec_times.push(read_val);
		read_val = in.csv();
		gpu_stp1_read_times.push(read_val);
		read_val = in.csv();
		gpu_stp2_write_times.push(read_val);
		read_val = in.csv();
		gpu_stp2_exec_times.push(read_val);
		read_val = in.csv();
		gpu_stp2_read_times.push(read_val);
	}
}

void draw_total_times() {
	new picture;
	size(50cm,30cm,false);
draw(graph(percentages,cpu_segura0_times),red,marker(scale(0.8mm)*unitcircle,red,FillDraw(red),above=false));
	draw(graph(percentages,cpu_stp0_times),blue,marker(scale(0.8mm)*unitcircle,blue,FillDraw(blue),above=false));
	draw(graph(percentages,cpu_stp1_times),green,marker(scale(0.8mm)*unitcircle,green,FillDraw(green),above=false));
	draw(graph(percentages,cpu_stp2_times),yellow,marker(scale(0.8mm)*unitcircle,yellow,FillDraw(yellow),above=false));
	draw(graph(percentages,gpu_segura0_exec_times),cyan,marker(scale(0.8mm)*unitcircle,cyan,FillDraw(cyan),above=false));
	draw(graph(percentages,gpu_stp0_exec_times),magenta,marker(scale(0.8mm)*unitcircle,magenta,FillDraw(magenta),above=false));
	draw(graph(percentages,gpu_stp1_exec_times),purple,marker(scale(0.8mm)*unitcircle,purple,FillDraw(purple),above=false));
	draw(graph(percentages,gpu_stp2_exec_times),orange,marker(scale(0.8mm)*unitcircle,orange,FillDraw(orange),above=false));

	xaxis("Ποσοστό Τεμνόμενων Ζευγών \%",BottomTop,LeftTicks);	
	yaxis("Χρόνος",LeftRight,RightTicks);
	string filename_total = filename + "_total";
	shipout(filename_total);
	erase();

}

void draw_time_per_test() {

	new picture;
	size(50cm,30cm,false);

draw(graph(percentages,cpu_segura0_times),red,marker(scale(0.8mm)*unitcircle,black,FillDraw(red),above=false));
	draw(graph(percentages,cpu_stp0_times),blue,marker(scale(0.8mm)*unitcircle,black,FillDraw(blue),above=false));
	draw(graph(percentages,cpu_stp1_times),green,marker(scale(0.8mm)*unitcircle,black,FillDraw(green),above=false));
	draw(graph(percentages,cpu_stp2_times),yellow,marker(scale(0.8mm)*unitcircle,black,FillDraw(yellow),above=false));
	draw(graph(percentages,gpu_segura0_exec_times),cyan,marker(scale(0.8mm)*unitcircle,black,FillDraw(cyan),above=false));
	draw(graph(percentages,gpu_stp0_exec_times),magenta,marker(scale(0.8mm)*unitcircle,black,FillDraw(magenta),above=false));
	draw(graph(percentages,gpu_stp1_exec_times),purple,marker(scale(0.8mm)*unitcircle,black,FillDraw(purple),above=false));
	draw(graph(percentages,gpu_stp2_exec_times),orange,marker(scale(0.8mm)*unitcircle,black,FillDraw(orange),above=false));

	xaxis("Ποσοστό Τεμνόμενων Ζευγών \%",BottomTop,LeftTicks);	
	yaxis("Χρόνος",LeftRight,RightTicks);
	string filename_per_test = filename + "_per_test";
	shipout(filename_per_test);
	erase();
}

get_input();

draw_total_times();

cpu_segura0_times =  time_per_test(cpu_segura0_times,total_pairs,repetitions);
cpu_stp0_times = time_per_test(cpu_stp0_times,total_pairs,repetitions);   	
cpu_stp1_times = time_per_test(cpu_stp1_times,total_pairs,repetitions);
cpu_stp2_times = time_per_test(cpu_stp2_times,total_pairs,repetitions);

gpu_segura0_write_times =  time_per_test(gpu_segura0_write_times,total_pairs,repetitions);
gpu_segura0_exec_times =  time_per_test(gpu_segura0_exec_times,total_pairs,repetitions);
gpu_segura0_read_times =  time_per_test(gpu_segura0_read_times,total_pairs,repetitions);

gpu_stp0_write_times =  time_per_test(gpu_stp0_write_times,total_pairs,repetitions);
gpu_stp0_exec_times =  time_per_test(gpu_stp0_exec_times,total_pairs,repetitions);
gpu_stp0_read_times =  time_per_test(gpu_stp0_read_times,total_pairs,repetitions);

gpu_stp1_write_times =  time_per_test(gpu_stp1_write_times,total_pairs,repetitions);
gpu_stp1_exec_times =  time_per_test(gpu_stp1_exec_times,total_pairs,repetitions);
gpu_stp1_read_times =  time_per_test(gpu_stp1_read_times,total_pairs,repetitions);

gpu_stp2_write_times =  time_per_test(gpu_stp2_write_times,total_pairs,repetitions);
gpu_stp2_exec_times =  time_per_test(gpu_stp2_exec_times,total_pairs,repetitions);
gpu_stp2_read_times =  time_per_test(gpu_stp2_read_times,total_pairs,repetitions); 

draw_time_per_test();


		 
		 
