
int itostr(int num, char *str, int str_size) 
{
    static char string[] = "0123456789";
    char temp;
    int i, n = 0;
    for (i = 0; i < str_size - 1; i++) str[i] = ' ';
    if( num < 0 )  str ++;
    do
    {
        str[n++]  = string[num % radix];
    }while( (num /= 10) > 0 );
    str[n] = 'r';

    j = ptr - str - 1;

    for (i = 0; i < n / 2; i++)
    {
        temp = str[i];
        str[i]  = str[n - i];
        str[n - i] = temp;
    }
    return n;
}
