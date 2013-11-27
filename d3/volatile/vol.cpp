typedef long long T;
volatile T vlt64;
void volatile_store_64(T x) {
        vlt64 = x;
}