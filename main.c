#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

char BASE58_CHAR[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9',
                      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

int char_table_index(char c)
{
    int size = sizeof(BASE58_CHAR) / sizeof(BASE58_CHAR[0]);

    for (int i = 0; i < size; i++)
    {
        if (BASE58_CHAR[i] == c)
            return i;
    }
    return 0;
}

char *reverse_string(char *s)
{
    int size = strlen(s);
    char *rev = malloc(size * sizeof(char));
    for (int i = 0; i < size; i++)
    {
        rev[size - 1 - i] = s[i];
    }

    return rev;
}

char *convret_int_to_string(int i)
{
    char *str = malloc(2 * sizeof(char));
    memset(str, '\0', 2);
    sprintf(str, "%d", i);
    return str;
}

typedef struct
{
    int *data;
    int size;
} bi;

bi *bi_new()
{
    bi *a = malloc(sizeof(bi));
    return a;
}

void bi_free(bi *a)
{
    free(a->data);
    free(a);
}

bi *bi_from_string(char *a)
{
    bi *b = bi_new();

    int skip = 0;
    while (a[skip] == '0')
    {
        skip++;
    }

    b->size = strlen(a) - skip;

    if (b->size == 0)
    {
        b->size++;
        b->data = malloc(b->size * sizeof(int));
        b->data[0] = 0;
    }
    else
    {
        b->data = malloc(b->size * sizeof(int));
        
        for (int i = 0; i < b->size; i++)
        {
            b->data[i] = a[skip + i] - '0';
        }
    }

    return b;
}

char *bi_to_string(bi *a)
{
    // if we don't add an extra element for the string
    // the result sometimes contains an extra character
    // so we make a string with one more element and we
    // set that element, '\0'.
    int  size = a->size;
    char *b = malloc((size+1) * sizeof(char));
    memset(b, '\0', size+1);
    
    for (int i = 0; i < size; i++)
        b[i] = a->data[i] + '0';
    
    return b;
}

bi *bi_add(bi *a, bi *b)
{
    bi *c = bi_new();
    // max possible size
    int size = (a->size > b->size ? a->size : b->size) + 1;
    int *sum = malloc(size * sizeof(int));
    for (int i = 0; i < size; i++)
        sum[i] = 0;
    
    int i = a->size - 1, j = b->size - 1;
    int k = size - 1;
    int carry = 0, tmp;

    while (i >= 0 || j >= 0 || carry > 0)
    {
        if (i >= 0 && j >= 0)
        {
            tmp = a->data[i] + b->data[j];
        }
        else if (i >= 0)
        {
            tmp = a->data[i];
        }
        else if (j >= 0)
        {
            tmp = b->data[j];
        }
        else
        {
            tmp = 0;
        }

        tmp += carry;
        carry = tmp / 10;
        sum[k] = tmp % 10;
        i--;
        j--;
        k--;
    }

    if (sum[0] == 0)
    {
        c->size = size - 1;
        c->data = malloc(c->size * sizeof(int));

        for (int i = 1; i < size; i++)
            c->data[i - 1] = sum[i];
    }
    else
    {
        c->size = size;
        c->data = malloc(size * sizeof(int));
        for (int i = 0; i < size; i++)
            c->data[i] = sum[i];
    }
    
    free(sum);

    // printf("%s + %s = %s\n", bi_to_string(a), bi_to_string(b), bi_to_string(c));

    return c;
}

bi *bi_multiply(bi *a, bi *b)
{
    int n = a->size, m = b->size;
    int size = n + m;
    int *v = malloc(size * sizeof(int));
    for (int i = 0; i < size; i++)
        v[i] = 0;

    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            v[size - 1 - (i + j)] += (a->data[n - 1 - i]) * (b->data[m - 1 - j]);

    int index = size - 1;

    while (index > 0)
    {
        if (v[index] >= 10)
        {
            v[index - 1] += v[index] / 10;
            v[index] = v[index] % 10;
        }
        index--;
    }
    
    int zero_num = 0;
    while (zero_num < size && v[zero_num] == 0)
        zero_num++;
        
    if (zero_num == size)
        return bi_from_string("0");

    bi *res = bi_new();
    res->size = size - zero_num;
    res->data = malloc((size - zero_num) * sizeof(int));
    for (int i = zero_num; i < size; i++)
        res->data[i - zero_num] = v[i];


    // printf("size: %d\tzero_num: %d\n", size, zero_num);
    // printf("res->size: %d\tzero_num: %d\n", res->size, size - zero_num);
    // printf("%s * %s = %s\n", bi_to_string(a), bi_to_string(b), bi_to_string(res));
    // printf("v: ");
    // for (int i = 0; i < size; i++)
    //     printf("%d", v[i]);
    // printf("\n");

    // 55083661177376864955426266318610878217089100
    // 55083661177376864955426266318610878217089100

    // 747483299640354875183129673956805670692693519632
    // 010747483299640354875183129673956805670692693519632
    // 55083661177376864955426266318610878217089100

    
    free(v);

    return res;
}

bool isSmaller(bi *a, bi *b)
{
    // Calculate lengths of both string
    int n1 = a->size, n2 = b->size;

    if (n1 < n2)
        return true;
    if (n2 < n1)
        return false;

    char *a_str = bi_to_string(a);
    char *b_str = bi_to_string(b);

    bool res = false;

    for (int i = 0; i < n1; i++)
    {
        if (a_str[i] < b_str[i])
        {
            res = true;
            break;
        }
        else if (a_str[i] > b_str[i])
        {
            res = false;
            break;
        }
    }

    free(a_str);
    free(b_str);

    return false;
}

void swap(bi *a, bi *b)
{
    bi temp = *a;
    *a = *b;
    *b = temp;
}

bi *bi_diff(bi *a, bi *b)
{
    if (isSmaller(a, b))
        swap(a, b);

    char *diff = malloc(a->size * sizeof(int));
    memset(diff, '\0', strlen(diff));

    int n1 = a->size, n2 = b->size;

    char *a_str = reverse_string(bi_to_string(a));
    char *b_str = reverse_string(bi_to_string(b));

    int carry = 0;

    for (int i = 0; i < n2; i++)
    {

        int sub = ((a_str[i] - '0') - (b_str[i] - '0') - carry);

        if (sub < 0)
        {
            sub = sub + 10;
            carry = 1;
        }
        else
            carry = 0;

        diff[i] = sub + '0';
    }

    for (int i = n2; i < n1; i++)
    {
        int sub = ((a_str[i] - '0') - carry);

        if (sub < 0)
        {
            sub = sub + 10;
            carry = 1;
        }
        else
            carry = 0;

        diff[i] = sub + '0';
    }

    char *diff_rev = reverse_string(diff);
    bi *res = bi_from_string(diff_rev);

    free(diff);
    free(a_str);
    free(b_str);
    free(diff_rev);

    return res;
}

int bi_mod(bi *a, int b)
{
    char *a_str = bi_to_string(a);
    int remainder = 0;
    int counter = 0;
    for (int i = 0; i < a->size; i++)
    {
        remainder *= 10;
        remainder += a_str[i] - '0';
        remainder %= b;
    }

    free(a_str);
    return remainder;
}

bi *bi_divide(bi *a, int b)
{
    char *ans = malloc(a->size * sizeof(int));
    memset(ans, '\0', strlen(ans));
    char *a_str = bi_to_string(a);

    // Find prefix of number that is larger
    // than divisor.
    int idx = 0;
    int temp = a_str[idx] - '0';
    while (temp < b)
    {
        temp = temp * 10 + (a_str[++idx] - '0');
    }

    // Repeatedly divide divisor with temp. After
    // every division, update temp to include one
    // more digit.
    int counter = 0;
    while (a->size > idx)
    {
        // Store result in answer i.e. temp / divisor
        ans[counter++] = (temp / b) + '0';

        // Take next digit of number
        temp = (temp % b) * 10 + a_str[++idx] - '0';
    }

    bi *res = bi_from_string(ans);
    free(ans);
    free(a_str);
    return res;
}

int get_string_number(char *s)
{
    int res = 0;
    int len = strlen(s);
    for (int i = 0; i < len; i++)
    {
        res *= 10;
        res += (int)(s[i] - '0');
    }
    return res;
}

char *convert_to_base58(char *s)
{
    bi *const58 = bi_from_string("58");
    bi *number = bi_from_string(s);

    char *res_reverse = malloc(50 * sizeof(char));
    int counter = 0;

    while (isSmaller(const58, number))
    {
        int mod = bi_mod(number, 58);
        bi *temp = bi_divide(number, 58);
        bi_free(number);
        number = temp;
        res_reverse[counter++] = BASE58_CHAR[mod];
    }
    // The last mod was not considered, so we apply this last mod
    res_reverse[counter] = BASE58_CHAR[bi_mod(number, 58)];

    char *res = reverse_string(res_reverse);

    bi_free(const58);
    bi_free(number);
    free(res_reverse);
    return res;
}

bi *convert_to_bigint(char *s)
{
    bi *const58 = bi_from_string("58");
    bi *number = bi_from_string("0");

    int size = strlen(s);
    for (int i = 0; i < size; i++)
    {
        bi *temp = number;
        number = bi_multiply(number, const58);
        bi_free(temp);
        
        int index = char_table_index(s[i]);

        bi *temp1 = bi_from_string(convret_int_to_string(index));
        bi *temp2 = number;

        number = bi_add(number, temp1);

        bi_free(temp1);
        bi_free(temp2);
    }

    bi_free(const58);

    return number;
}

int main()
{
    printf("Hello World\n");

    char *res = convert_to_base58("411404634311556578553424950164267302198352612827866539517561");
    printf("%s\n", res);
    assert(strcmp("TNatD91C68BGnncnchidC8WJDENFjTXAoz", res) == 0);
    printf("Finished Reached.\n");

    // 411404634311556578553424950164267302198352612827866539517561 => TNatD91C68BGnncnchidC8WJDENFjTXAoz

    // 409142637355783295518398059253544599013955861107106666027453 => TEB6xN41zcajmUDeCwjp3HPbSqMTfA42wN

    bi *val = convert_to_bigint("TEB6xN41zcajmUDeCwjp3HPbSqMTfA42wN");
    char *val_str = bi_to_string(val);
    char *correct_val = "409142637355783295518398059253544599013955861107106666027453";
    printf("val: %s\n", val_str);
    assert(strcmp("409142637355783295518398059253544599013955861107106666027453", val_str) == 0);
    printf("Finished Reached.\n");

    return 0;
}
