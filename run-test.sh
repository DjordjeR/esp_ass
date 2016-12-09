#! /bin/bash

gcc -Wall -o bin assa.c
./bin < tests/add_tests.txt > test_outputs/add_tests_out.txt
./bin < tests/draw-all_tests.txt > test_outputs/draw-all_tests_out.txt
./bin < tests/list_tests.txt > test_outputs/list_tests_out.txt
echo "Testovi bez fajla zavrseni";
./bin black_fam-tree.dot < tests/add_tests.txt > test_outputs/add_tests_file_out.txt
./bin black_fam-tree.dot < tests/draw-all_tests.txt > test_outputs/draw-all_tests_file_out.txt
./bin black_fam-tree.dot < tests/list_tests.txt > test_outputs/list_tests_file_out.txt
echo "Testovi sa ucitanim fajlom zavrseni";
valgrind ./bin < tests/add_tests.txt > test_outputs/add_tests_valgrind_out.txt
valgrind ./bin < tests/draw-all_tests.txt > test_outputs/draw-all_tests_valgrind_out.txt
valgrind ./bin < tests/list_tests.txt > test_outputs/list_tests_valgrind_out.txt
echo "Testovi sa valgrind bez fajla zavrseni";
valgrind ./bin black_fam-tree.dot < tests/add_tests.txt > test_outputs/add_tests_valgrind_file_out.txt
valgrind ./bin black_fam-tree.dot < tests/draw-all_tests.txt > test_outputs/draw-all_tests_valgrind_file_out.txt
valgrind ./bin black_fam-tree.dot < tests/list_tests.txt > test_outputs/list_tests_valgrind_file_out.txt
echo "Testovi sa valgrind sa fajlom zavrseni";