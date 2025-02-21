#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}



@test "Change directory to absolute path" {
    current=$(pwd)
    
    cd /tmp
    mkdir -p dsh-test

    run "${current}/dsh" <<EOF
cd /tmp/dsh-test
pwd
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="/tmp/dsh-testdsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Change directory to relative path" {
    current=$(pwd)
    
    cd /tmp
    mkdir -p dsh-test

    run "${current}/dsh" <<EOF
cd /tmp
cd dsh-test
pwd
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="/tmp/dsh-testdsh2>dsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Change directory to non-existent directory" {
    current=$(pwd)

    run "${current}/dsh" <<EOF
cd /tmp/does-not-exist
EOF

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$status" -eq 0 ]  # Expecting a failure
}

@test "Change directory with too many arguments" {
    current=$(pwd)

    run "${current}/dsh" <<EOF
cd /tmp /var
EOF

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$status" -eq 0 ]  # Expecting a failure
}

@test "Exit command without arguments works" {
    current=$(pwd)

    run "${current}/dsh" <<EOF
exit
EOF

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$status" -eq 0 ]  # Shell should exit normally
}


@test "Exit command with arguments just exits" {
    current=$(pwd)

    run "${current}/dsh" <<EOF
exit abc
EOF

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$status" -eq 0 ]  
}