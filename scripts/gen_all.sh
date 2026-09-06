#!/usr/bin/env bash
set -euo pipefail

types=(
	"SO(4,3)"
	"SU(4,3)"
	"G2_split"
	"F4_split"
	"E6_quaternionic"
	"E6_hermitian"
	"E7_split"
	"E7_quaternionic"
	"E7_hermitian"
	"E8_split"
	"E8_quaternionic"
)

for type in "${types[@]}"; do
	result_dir="./results/${type}"
	raw_dir="${result_dir}/raw_embedding"
	dot_dir="${result_dir}/dot"
	associated_dot_dir="${result_dir}/associated_dot"
	svg_dir="${result_dir}/svg"
	associated_svg_dir="${result_dir}/associated_svg"

	mkdir -p "$raw_dir" "$dot_dir" "$associated_dot_dir" "$svg_dir" "$associated_svg_dir"
	rm -rf "$raw_dir"/* "$dot_dir"/* "$associated_dot_dir"/* "$svg_dir"/* "$associated_svg_dir"/*

	echo "=== searching embeddings for ${type} ==="
	./build/search_embeddings "$type" "$raw_dir"

	shopt -s nullglob
	for txt_file in "${raw_dir}"/*.txt; do
		base="$(basename "$txt_file" .txt)"
		dot_file="${dot_dir}/${base}"
		./build/convert_to_dot "$txt_file" "$dot_file" --remove-noncompact
		./build/embedding_in_associated_dot "$txt_file" "${associated_dot_dir}/${base}"
	done
	for dot_file in "${dot_dir}"/*.dot; do
		base="$(basename "$dot_file" .dot)"
		svg_file="${svg_dir}/${base}.svg"
		dot -Tsvg -o "$svg_file" "$dot_file"
	done
	for associated_dot_file in "${associated_dot_dir}"/*.dot; do
		base="$(basename "$associated_dot_file" .dot)"
		associated_svg_file="${associated_svg_dir}/${base}.svg"
		dot -Tsvg -o "$associated_svg_file" "$associated_dot_file"
	done
	shopt -u nullglob
done