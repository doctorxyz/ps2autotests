#include "shared.h"

#define RRR_OP_FUNC(OP) \
static inline void RRR_##OP(register u128 &rd, const register u128 &rs, const register u128 &rt) { \
	asm volatile ( \
		#OP " %0, %1, %2\n" \
		: "+r"(rd) : "r"(rs), "r"(rt) \
	); \
} \
static inline void ZRR_##OP(register u128 &rd, const register u128 &rs, const register u128 &rt) { \
	asm volatile ( \
		#OP " $0, %1, %2\n" \
		"por %0, $0, $0\n" \
		: "+r"(rd) : "r"(rs), "r"(rt) \
	); \
}

#define RRR_OP_DO_III(OP, d, s, t) \
	SET_U32<d>(rd); SET_U32<s>(rs); SET_U32<t>(rt); \
	RRR_##OP(rd, rs, rt); \
	printf("  %s %d, %d: ", #OP, (u32)rs, (u32)rt); PRINT_R(rd, true);

#define RRR_OP_DO_MMM(OP, d, s, t) \
	SET_M(rd, d); SET_M(rs, s); SET_M(rt, t); \
	RRR_##OP(rd, rs, rt); \
	printf("  %s %s, %s: ", #OP, #s, #t); PRINT_R(rd, true);

#define TEST_RRR_FUNC(OP) \
RRR_OP_FUNC(OP); \
static void test_##OP() { \
	register u128 rd, rs, rt; \
	 \
	printf("%s:\n", #OP); \
	 \
	RRR_OP_DO_III(OP, 0x1337, 0, 0); \
	RRR_OP_DO_III(OP, 0x1337, 0, 1); \
	RRR_OP_DO_III(OP, 0x1337, 1, 1); \
	RRR_OP_DO_III(OP, 0x1337, 1, 0); \
	RRR_OP_DO_III(OP, 0x1337, 2, 2); \
	RRR_OP_DO_III(OP, 0x1337, 0xFFFFFFFF, 1); \
	RRR_OP_DO_III(OP, 0x1337, 0xFFFFFFFF, 0xFFFFFFFF); \
	 \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_ZERO, C_ZERO); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_ZERO, C_ONE); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_ONE, C_ZERO); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_ONE, C_ONE); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_ONE, C_NEGONE); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_S16_MAX, C_S16_MAX); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_S16_MIN, C_S16_MIN); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_S32_MAX, C_S32_MAX); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_S32_MIN, C_S32_MIN); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_S64_MAX, C_S64_MAX); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_S64_MIN, C_S64_MIN); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_GARBAGE1, C_GARBAGE2); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_P_S8_A, C_P_S8_B); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_P_S16_A, C_P_S16_B); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_P_S32_A, C_P_S32_B); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_P_S32_B, C_P_S32_A); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_P_S32_C, C_P_S32_D); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_P_S32_D, C_P_S32_C); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_P_S32_E, C_P_S32_F); \
	RRR_OP_DO_MMM(OP, C_GARBAGE1, C_P_S32_F, C_P_S32_E); \
	\
	SET_M(rd, C_GARBAGE1); SET_M(rs, C_GARBAGE1); SET_M(rt, C_GARBAGE1); \
	ZRR_##OP(rd, rs, rt); \
	printf("  %s -> $0: ", #OP); PRINT_R(rd, true); \
	printf("\n"); \
}

TEST_RRR_FUNC(pceqb)
TEST_RRR_FUNC(pceqh)
TEST_RRR_FUNC(pceqw)
TEST_RRR_FUNC(pcgtb)
TEST_RRR_FUNC(pcgth)
TEST_RRR_FUNC(pcgtw)

int main(int argc, char **argv) {
	printf("-- TEST BEGIN\n");

	test_pceqb();
	test_pceqh();
	test_pceqw();
	test_pcgtb();
	test_pcgth();
	test_pcgtw();

	printf("-- TEST END\n");

	return 0;
}
