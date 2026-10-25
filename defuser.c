#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    (void)challenge;
    uint32_t left = (uint32_t)(challenge >> 32);
    uint32_t right = (uint32_t)(challenge);
    uint32_t mid = (uint32_t)(challenge >> 16);
    uint32_t p = (left + 0x6960) ^ right;
    uint32_t q = ((left * 3) + mid) ^ 0x88c3;
    uint32_t ans = (p * 4) + (q ^ 0xa42f);
    values[0] = ans;
    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 6;
    (void)challenge;
    uint32_t q = (uint32_t)(challenge >> 32);
    values[0] = ((q+q)&0x1FE)+0x15;
    int shf = 24;
    uint32_t p = values[0];
    for (int i = 1; i < 6; i++) {
        uint32_t t = (uint32_t)((challenge >> shf) & 0xF);
        t=t+i;
        values[i]=t^(p*3);
        p=values[i];
        shf+=4;
    }
    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;
    (void)challenge;
    uint32_t ans1 = (uint32_t)(challenge&0x7);
    uint32_t left  = (uint32_t)(challenge >> 32);
    uint32_t right = (uint32_t)(challenge >> 3);
    uint32_t p = left^right;
    uint32_t ans2;
    switch (ans1) {
        case 0: ans2 = p * 5 + 0x4785b;   break;
        case 1: ans2 = (p * 2) ^ 0xf833;  break;
        case 2: ans2 = p * 5+ 0x3c26; break;
        case 3: ans2 = (p ^ 0x21de) + (p >> 2); break;
        case 4: ans2 = (p * 4) ^ 0x7b23; break;
        case 5: ans2 = p * 5 + 0x289c4; break;
        case 6: ans2 = p * 3 + 0x1b1c; break;
        case 7: ans2 =(p ^ 0x2474) + (p >> 2); break;
    }
    values[0]=ans1;
    values[1]=ans2;
    return write_answer(out, cap, values, count);
}

uint32_t p4_walk(uint32_t t, uint32_t l, uint32_t h, uint32_t s) {
    uint32_t m = l + (h - l) / 2;
    if (m == t) {
        return (m+0x87u)^s;
    }
    if (t > m) {
        uint32_t r = p4_walk(t,m+1,h,s);
        return (r*5u)^(m+s)^0x15u;
    } else {
        uint32_t r = p4_walk(t,l,m-1,s);
        return (r*5u)^(m+s)^0x1fu;
    }
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    (void)challenge;
     uint32_t t = (uint32_t)(challenge & 0x7fu);
    uint32_t s = (uint32_t)((challenge>>7)^(challenge>>32));
    values[0] = p4_walk(t, 0, 0x7fu, s);
    return write_answer(out, cap, values, count);
}

typedef struct Node {
    int next;
    uint32_t v1;
    uint32_t v2;
} Node;
 Node N[12] = {
        {6,2157,52050},
        {8,2436,57992},
        {5,1127,22681},
        {2,562,574},
        {-1,1352,8311},
        {11,1639,51600},
        {3,2591,38286},
        {9,2859,8457},
        {10,1896,24709},
        {4,844,50633},
        {0,284,1941},
        {7,74,41951}
    };
int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;
    (void)challenge;
    int h[8] = {1, 8, 10, 0, 6, 3, 2, 5};
    
    uint32_t key = challenge & 0x7;
    uint32_t j = (challenge>>3) & 0x3;
    uint32_t a = (uint32_t)((challenge >> 6)^(challenge >> 32));
    int i = h[key];
    uint32_t ans1 = (a^N[i].v2^0x3677u) + N[i].v1;
    uint32_t ans2=N[i].v1;
 while (j > 0) {
        i = N[i].next;
        ans1 = (ans1^N[i].v2)+N[i].v1;
        ans2=N[i].v1;
        j--;
    }
    values[0] = ans1;
    values[1] = ans2;
    return write_answer(out, cap, values, count);
}


uint32_t f0(uint32_t x) { return ((x << 1) | (x >> 31)) ^ 0xab1du; }
uint32_t f1(uint32_t x) { return ((x >> 5) ^ x) + 0x34d0u; }
uint32_t f2(uint32_t x) { return (x + 0x740du) ^ (x >> 1); }
uint32_t f3(uint32_t x) { return (x * 4u + 0x7cacu) ^ (x * 5u); }
uint32_t f4(uint32_t x) { return (x >> 7) | (x << 25); }
uint32_t f5(uint32_t x) { return (x ^ 0xbbe8u) + (x << 5); }
uint32_t dispatch_p6(uint32_t idx, uint32_t x) {
    switch (idx) {
        case 0: return f5(x);
        case 1: return f3(x);
        case 2: return f1(x);
        case 3: return f1(x);
        case 4: return f2(x);
        case 5: return f1(x);
        case 6: return f4(x);
        case 7: return f0(x);
    }
    return 0;
}
int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    (void)challenge;

    uint32_t i1 = challenge & 0x7;
    uint32_t i2 = (challenge >> 3) & 0x7;
    uint32_t a = (uint32_t)((challenge >> 6) ^ (challenge >> 32));
    uint32_t r1 = dispatch_p6(i1, a);
    uint32_t r2 = dispatch_p6(i2, r1^0x31a6u);
    values[0] = r2 + 0x361bu;
    return write_answer(out, cap, values, count);
}


uint32_t Mix(uint32_t x) {
    return (((x << 1) | (x >> 31)) + 0xede7u) ^ 0x1b0u;
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    (void)challenge;
    (void)context;
    (void)out;
    (void)cap;
    // TODO（选做）: 根据 secret_gate、secret_copy 和 secret_phase 的汇编构造完整的第七行，写入 out 后返回 0。
    // 第七行不一定只有整数，可以直接用 snprintf 写入；还没完成时保持返回 1，测试会显示 not attempted。
    uint64_t c1 = challenge;
    uint64_t c2 = context;
    
    uint32_t value0=(uint32_t)(c2 & 0x3u);
    
    uint32_t s=(uint32_t)((c1 >> 32)^c1);
    uint32_t t=(uint32_t)((c2 >> 32)^c2);
    
    uint32_t x = s^t;
    uint32_t y= Mix(x);
    uint32_t yu= (value0*0xede7u)^(uint32_t)(c2 >> 16);
    uint32_t value1 =y^yu;
    uint32_t temp=0x1b0u+t+value0+ (s>> 1);
    uint32_t value2 = value1^temp;
    snprintf(out,cap,"abcdefghijklmnopqrstuvwxyzabcdefOPEN%u %u %u",value0, value1, value2);
    return 0;
}
