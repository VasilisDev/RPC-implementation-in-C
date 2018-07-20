
struct data_in {
int array<>;
int ar_length;
double x_multiplier;
};
struct data_out{
int array_min_max[2];
int ar_length;
double multi_array<>;
};
program PROG{
version VER1 {
data_out minmax(data_in)=1;
double avg(data_in)=2;
data_out multiply(data_in)=3;
}=1;
}=0x30090949;
