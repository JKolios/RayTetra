import graph;
import unicode;
texpreamble("\usepackage[greek]{babel} ");
defaultpen(fontsize(18pt));
     
  size(50cm,30cm,IgnoreAspect);

     
  file in=(input("bench_output.csv",true,"#"));
  
  //Actual graph data  
  int cpu_segura0_times[];
  int cpu_stp0_times[];  
  int cpu_stp1_times[];
  int cpu_stp2_times[];

  int gpu_segura0_write_times[];
  int gpu_segura0_exec_times[];
  int gpu_segura0_read_times[];

  int gpu_stp0_write_times[];
  int gpu_stp0_exec_times[];
  int gpu_stp0_read_times[];

  int gpu_stp1_write_times[];
  int gpu_stp1_exec_times[];
  int gpu_stp1_read_times[];

  int gpu_stp2_write_times[];
  int gpu_stp2_exec_times[];
  int gpu_stp2_read_times[];

  int read_val;

  //Calculates the number of input lines
  int gran = in.csv();
  real point_count =  100 / gran + 1;
  write("Point Count:",point_count);
  int percentages[];
  for(int i=0;i<point_count;++i) percentages.push(i * gran);
    
  
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

draw(graph(percentages,cpu_segura0_times),red,marker(scale(0.8mm)*unitcircle,red,FillDraw(red),above=false));
draw(graph(percentages,cpu_stp0_times),blue,marker(scale(0.8mm)*unitcircle,blue,FillDraw(blue),above=false));
draw(graph(percentages,cpu_stp1_times),green,marker(scale(0.8mm)*unitcircle,green,FillDraw(green),above=false));
draw(graph(percentages,cpu_stp2_times),yellow,marker(scale(0.8mm)*unitcircle,yellow,FillDraw(yellow),above=false));
draw(graph(percentages,gpu_segura0_exec_times),cyan,marker(scale(0.8mm)*unitcircle,cyan,FillDraw(cyan),above=false));
draw(graph(percentages,gpu_stp0_exec_times),magenta,marker(scale(0.8mm)*unitcircle,magenta,FillDraw(magenta),above=false));
draw(graph(percentages,gpu_stp1_exec_times),purple,marker(scale(0.8mm)*unitcircle,purple,FillDraw(purple),above=false));
draw(graph(percentages,gpu_stp2_exec_times),orange,marker(scale(0.8mm)*unitcircle,orange,FillDraw(orange),above=false));
xaxis("Ποσοστό Τεμνόμενων Ζευγών \%",BottomTop,LeftTicks);  
yaxis("Χρόνος (ms)",LeftRight,RightTicks);
     
     
