#include <catch2/catch.hpp>
#include <cyclic_buffer.hpp>

TEST_CASE("cyclic_buffer") {
	CyclicBuffer<int, 4> cb;
	CHECK(cb.empty());
	CHECK(cb.size() == 0);

	for (int i = 0; i < 4; ++i) {
		CHECK(!cb.full());
		CHECK(cb.size() == i);
		CHECK(cb.capacity() == 4);

		auto &tmp = cb.push(i);

		CHECK(cb.size() == i + 1);
		CHECK(&cb.back() == &tmp);
		CHECK(&cb.back() == &cb[i]);
		CHECK(!cb.empty());
	}
	for (int i = 0; i < 4; ++i)
		CHECK(cb[i] == i);

	CHECK(cb.full());
	CHECK(cb.size() == 4);

	cb.push(4);
	CHECK(cb.full());
	CHECK(cb[0] == 1);
	CHECK(cb[1] == 2);
	CHECK(cb[2] == 3);
	CHECK(cb[3] == 4);

	cb.clear();
	CHECK(cb.size() == 0);
	CHECK(cb.empty());
	CHECK(!cb.full());

	cb.push(5);
	CHECK(cb[0] == 5);

	CyclicBuffer<int[3], 4> cb2;
	cb2.push();
	cb2[0][0] = 3;
	CHECK(cb2[0][0] == 3);
	CHECK(!cb2.empty());
}
