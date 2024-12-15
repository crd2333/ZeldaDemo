#!/bin/bash
set -e

if [ ! -d "terrain" ]; then
  mkdir "terrain"
fi

for x in *.hght.sstera; do
  wszst dec "$x" --dest "${x%.hght.sstera}.stera"
  sarc x "${x%.hght.sstera}.stera" -C "terrain"
  rm "${x%.hght.sstera}.stera"
done