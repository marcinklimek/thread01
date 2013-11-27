volatile bool volatile_bool_done;
double regular_double;

void volatile_example_2() 
{
        regular_double = 1.23;
        volatile_bool_done = true;
}