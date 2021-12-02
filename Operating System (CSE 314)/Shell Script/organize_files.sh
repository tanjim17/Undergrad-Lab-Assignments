#!/bin/bash

usage_msg="1st arg: working directory (optional. if not given, directory containing this script will be chosen)\n2nd arg: file containing list of types to be ignored\n"

organize() {
    if [ ! -d $1 ] || [ ! -f $2 ]; then
        printf "no such file or directory!\n$usage_msg"
        return
    fi

    sed 's/\r$//' $2 > temp_input.txt
    readarray -t ignored_types < temp_input.txt
    
    output_dir="output_dir"
    if [ ! -d "$output_dir" ]; then
        mkdir "$output_dir"
    fi

    declare -A count
    count["ignored"]="0"

    for path in $(find $1 -type f); do
        filename=$(basename "$path")
        extension=${filename##*.}

        is_ignored="false"
        for type in "${ignored_types[@]}"; do
            if [ "$extension" = "$type" ]; then
                is_ignored="true"
                count["ignored"]=$((${count["ignored"]} + 1))
                break
            fi
        done
        if [ "$is_ignored" = "true" ]; then
            continue
        fi

        if [ "$filename" = "$extension" ]; then
            subdir="others"
        else
            subdir="$extension"
        fi
        dest="$output_dir/$subdir"
        
        if [ ! -d "$dest" ]; then
            mkdir "$dest"
            touch "$dest/desc_$subdir.txt"
            count["$subdir"]="0"
        fi
        if [ ! -f "$dest/$filename" ]; then
            cp "$path" "$dest"
            echo $path >> "$dest/desc_$subdir.txt"
            count["$subdir"]=$((${count["$subdir"]} + 1))
        fi
    done

    echo "file_type,no_of_files" > output.csv
    for subdir in ${!count[@]}; do
        echo "${subdir},${count["${subdir}"]}" >> output.csv
    done

    rm temp_input.txt
}

if [ $# -eq 1 ]; then
    organize . $1
elif [ $# -eq 2 ]; then
    organize $1 $2
else
    printf "missing or too many arguments!\n$usage_msg"
fi