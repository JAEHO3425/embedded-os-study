// mmu_sim.c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define PHYS_MEM_SIZE (64 * 1024)   

typedef struct {
    uint32_t base;     // 물리 메모리 시작 주소
    uint32_t bounds;   // 허용되는 가상 주소 크기
} TZASC;

/// 64KB 물리 메모리
static uint8_t phys_mem[PHYS_MEM_SIZE];

// 가상 주소 -> 물리 주소 변환 (Base & Bounds)
static uint32_t translate(TZASC config, uint32_t v_addr) {
    // bounds 체크: v_addr는 0 ~ bounds-1만 허용
    if (v_addr >= config.bounds) {
        printf("[FAULT] Address %u is Out of Bounds! (bounds=%u)\n",
               v_addr, config.bounds);
        exit(1); // 즉시 종료
    }

    // 물리 주소 계산
    uint32_t p_addr = config.base + v_addr;

    // 물리 메모리 전체 범위도 넘어가면 오류
    if (p_addr >= PHYS_MEM_SIZE) {
        printf("[FAULT] Physical Address %u is Out of Physical Memory! (size=%u)\n",
               p_addr, (unsigned)PHYS_MEM_SIZE);
        exit(1);
    }

    return p_addr;
}

// 1바이트 쓰기 예시
static void mmu_write8(TZASC config, uint32_t v_addr, uint8_t value) {
    uint32_t p_addr = translate(config, v_addr);
    phys_mem[p_addr] = value;
    printf("[WRITE] v_addr=%u -> p_addr=%u, value=%u\n", v_addr, p_addr, value);
}

// 1바이트 읽기 예시
static uint8_t mmu_read8(TZASC config, uint32_t v_addr) {
    uint32_t p_addr = translate(config, v_addr);
    uint8_t value = phys_mem[p_addr];
    printf("[READ ] v_addr=%u -> p_addr=%u, value=%u\n", v_addr, p_addr, value);
    return value;
}

int main(void) {
    // 64KB 물리 메모리 초기화
    for (uint32_t i = 0; i < PHYS_MEM_SIZE; i++) {
        phys_mem[i] = 0;
    }

    // base=32KB, bounds=16KB
    TZASC secure_partition = { 32 * 1024, 16 * 1024 };

    // 정상 접근: v_addr=100
    mmu_write8(secure_partition, 100, 42);
    mmu_read8(secure_partition, 100);

    // bounds 초과 접근: v_addr=20000 -> 즉시 종료 + 로그 남기기
    mmu_read8(secure_partition, 20000);

    return 0;
}