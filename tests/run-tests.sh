set -e
set -o pipefail

no_err=$1

function run()
{
    set +e

    echo "test $1"
    out=$(python3 check.py $1 2>&1)
    if [[ ! $? -eq 0 ]];
    then
        if [[ -z ${no_err} ]];
        then
            echo "test $1 failed, please rerun it with:"
            echo "    python3 check.py $1"
            echo "detail:"
            echo "${out}"
            exit 1
        else
            echo "    FAIL test $1"
        fi
    fi
}

for cpp in *.cpp;
do
    run "${cpp}"
done