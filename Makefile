.SILENT: test a.out compare clean

all: a.out compare

a.out : main.c codes_counter.c file_write.c huffman_dict.c huffman_tree.c min_heap.c node.c progress.c
	cc main.c codes_counter.c file_write.c huffman_dict.c huffman_tree.c min_heap.c node.c progress.c -lm

compare : compare_files.c
	cc compare_files.c -o compare

test: a.out compare
	@for i in `seq 1 20`; do \
		tr -dc "A-Za-z 0-9" < /dev/urandom | fold -w1048575 | head -n 10 > "test$$i.txt"; \
	done
	./a.out -z test1.txt -c test1_compressed -x test1_compressed -d test1_decompressed.txt; \
	./compare test1.txt test1_decompressed.txt;

	./a.out -z test2.txt -c test2_compressed -x test2_compressed -d test2_decompressed.txt; \
	./compare test2.txt test2_decompressed.txt;

	./a.out -z test3.txt -c test3_compressed -x test3_compressed -d test3_decompressed.txt; \
	./compare test3.txt test3_decompressed.txt;

	./a.out -z test4.txt -c test4_compressed -x test4_compressed -d test4_decompressed.txt -o1; \
	./compare test4.txt test4_decompressed.txt;

	./a.out -z test5.txt -c test5_compressed -x test5_compressed -d test5_decompressed.txt -o1; \
	./compare test5.txt test5_decompressed.txt;

	./a.out -z test6.txt -c test6_compressed -x test6_compressed -d test6_decompressed.txt -o1; \
	./compare test6.txt test6_decompressed.txt;

	./a.out -z test7.txt -c test7_compressed -x test7_compressed -d test7_decompressed.txt -o2; \
	./compare test7.txt test7_decompressed.txt;

	./a.out -z test8.txt -c test8_compressed -x test8_compressed -d test8_decompressed.txt -o2; \
	./compare test8.txt test8_decompressed.txt;

	./a.out -z test9.txt -c test9_compressed -x test9_compressed -d test9_decompressed.txt -o2; \
	./compare test9.txt test9_decompressed.txt;

	./a.out -z test10.txt -c test10_compressed -x test10_compressed -d test10_decompressed.txt -o3; \
	./compare test10.txt test10_decompressed.txt;

	./a.out -z test11.txt -c test11_compressed -x test11_compressed -d test11_decompressed.txt -o3; \
	./compare test11.txt test11_decompressed.txt;

	./a.out -z test12.txt -c test12_compressed -x test12_compressed -d test12_decompressed.txt -o3; \
	./compare test12.txt test12_decompressed.txt;

	./a.out -z test13.txt -c test13_compressed -x test13_compressed -d test13_decompressed.txt –p haslo123; \
	./compare test13.txt test13_decompressed.txt;

	./a.out -z test14.txt -c test14_compressed -x test14_compressed -d test14_decompressed.txt –p testowehaslo; \
	./compare test14.txt test14_decompressed.txt;

	./a.out -z test15.txt -c test15_compressed -x test15_compressed -d test15_decompressed.txt -o1 –p haslo123; \
	./compare test15.txt test15_decompressed.txt;

	./a.out -z test16.txt -c test16_compressed -x test16_compressed -d test16_decompressed.txt -o1 –p testowehaslo; \
	./compare test16.txt test16_decompressed.txt;

	./a.out -z test17.txt -c test17_compressed -x test17_compressed -d test17_decompressed.txt -o2 –p haslo123; \
	./compare test17.txt test17_decompressed.txt;

	./a.out -z test18.txt -c test18_compressed -x test18_compressed -d test18_decompressed.txt -o2 –p testowehaslo; \
	./compare test18.txt test18_decompressed.txt;

	./a.out -z test19.txt -c test19_compressed -x test19_compressed -d test19_decompressed.txt -o3 –p haslo123; \
	./compare test19.txt test19_decompressed.txt;

	./a.out -z test20.txt -c test20_compressed -x test20_compressed -d test20_decompressed.txt -o3 –p testowehaslo; \
	./compare test20.txt test20_decompressed.txt;

clean:
	@for i in `seq 1 20`; do \
		rm -f "test$$i.txt" "test$$i"_compressed "test$$i"_decompressed.txt; \
	done
	rm -f *.o a.out compare








