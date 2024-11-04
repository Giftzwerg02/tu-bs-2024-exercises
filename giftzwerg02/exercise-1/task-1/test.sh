#!/usr/bin/env bash

usage_msg=$(cat << EOF
Ayo bro you ain't using this shit right.
mygrep [-i] [-o outfile] keyword [file...]
Options:
> -i (case insensitive)
> -o [outfile] (output file)
EOF
)

file_does_not_exist_msg=$(cat << EOF
error: file does not exist
EOF
)

file_cannot_be_written_to_msg=$(cat << EOF
error: file cannot be written to
EOF
)

GREEN=$'\033[0;32m'
RED=$'\033[0;31m'
NC=$'\033[0m' # No Color

function setup() {
	make clean && make
}

function teardown() {
	make clean
}

function run_tests() {
	local test_name="$1"

	if [ -z "$test_name" ]; then
		usage1
		usage2
		usage3
		usage4

		easy1
		easy2
		easy3
		easy4

		file1
		file2
		file3

		longline

		fileerror1
		fileerror2
	else
		$test_name
	fi
}

function usage1() {
	res=$(./mygrep -x)
	assert 1 $? "usage1: should have exit code = 1"
	assert "${usage_msg}" "${res}" "usage1: should print usage message"
}

function usage2() {
	res=$(./mygrep -i -i)
	assert 1 $? "usage2: should have exit code = 1"
	assert "${usage_msg}" "${res}" "usage2: should print usage message"
}

function usage3() {
	res=$(./mygrep -o)
	assert 1 $? "usage3: should have exit code = 1"
	assert "${usage_msg}" "${res}" "usage3: should print usage message"
}

function usage4() {
	res=$(./mygrep)
	assert 1 $? "usage4: should have exit code = 1"
	assert "${usage_msg}" "${res}" "usage4: should print usage message"
}

function easy1() {
	res=$(echo -e "Systems, welcome\nto\noperating systems.\n" | ./mygrep "sys")
	assert 0 $? "easy1: should have exit code = 0"
	assert "operating systems." "${res}" "easy1: line should be matched"
}

function easy2() {
	res="$(echo -e "Small\nis smaller\nthan huge." | ./mygrep -i "small")"
	assert 0 $? "easy2: should have exit code = 0"
	assert $'Small\nis smaller' "${res}" "easy2: lines should be matched"
}

function easy3() {
	res="$(echo -e "Oh, z.,\nZZZzZ.\nz\nZ\nZZZz\nzZ...." | ./mygrep -i "Z.")"
	assert 0 $? "easy3: should have exit code = 0"
	assert $'Oh, z.,\nZZZzZ.\nzZ....' "${res}" "easy3: lines should be matched"
}

function easy4() {
	res="$(echo -e "new\nline" | ./mygrep "")"
	assert 0 $? "easy4: should have exit code = 0"
	assert $'new\nline' "${res}" "easy4: lines should be matched"
}

function file1() {
	echo -e "2 is the oddest prime.\nwhat?\nAre YOU sure\n12345, yes." > infile.txt

	res="$(./mygrep -o outfile.txt 2 infile.txt)"
	assert 0 $? "file1: should have exit code = 0"

	file="$(cat outfile.txt)"
	assert $'2 is the oddest prime.\n12345, yes.' "${file}" "file1: lines should be matched"

	rm infile.txt outfile.txt
}

function file2() {
	echo '.,:-!=?% is %n%% TEST???' > infile1
	echo '.,:-!=?% is %n%% yes!!!' >> infile1
	echo -e "testing\ntester\nTESTEST\nteeest\nabc\ndef" > infile2

	res="$(./mygrep -i Test infile1 infile2)"
	assert 0 $? "file2: should have exit code = 0"
	assert $'.,:-!=?% is %n%% TEST???\ntesting\ntester\nTESTEST' "${res}" "file2: lines should be matched"

	rm infile1 infile2
}

function file3() {
	echo "some old content" > outfile
	
	res="$(echo -e "abcdef\nabbcdef\nabscdef\nabecdef\naabcdef\n" | ./mygrep -i -o outfile "ABC")"
	assert 0 $? "file3: should have exit code = 0"

	file=$(cat outfile)
	assert $'abcdef\naabcdef' "${file}" "file3: lines should be matched"

	rm outfile
}

function longline() {
	( echo -n "yes"; printf -- "-%.0s" {1..8000}; echo "..." ) > longline
	
	res="$(./mygrep -i "yes" longline > longgrep)"
	assert 0 $? "longline: should have exit code = 0"

	file=$(cat longgrep | tr -d "-")
	assert $'yes...' "${file}" "longline: lines should be matched"

	rm longline longgrep
}

function fileerror1() {
	rm -rf nonExistingTestfile
	
	res="$(./mygrep test nonExistingTestfile)"
	assert 1 $? "fileerror1: should have exit code = 1"
	assert "${file_does_not_exist_msg}" "${res}" "fileerror1: lines should be matched"
}

function fileerror2() {
	touch existingTestfile
	chmod 0000 existingTestfile

	res="$(echo "test" | ./mygrep -o existingTestfile test)"
	assert 1 $? "fileerror2: should have exit code = 1"
	assert "${file_cannot_be_written_to_msg}" "${res}" "fileerror2: lines should be matched"

	chmod 0700 existingTestfile
	rm existingTestfile
}


# Function to handle assertions
assert() {
    local expected="$1"
    local actual="$2"
    local message="$3"

    # Check if expected and actual values are equal
    if [ "$expected" = "$actual" ]; then
        echo "${GREEN}${message}: OK${NC}"
    else
        echo "${RED}${message}: FAILED (expected: <$expected>, actual: <$actual>)${NC}"
    fi
}

setup
echo "---- STARTING TESTS ----"
run_tests "$@"
echo "----  TESTING DONE  ----"
teardown
