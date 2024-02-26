# Notest

## Optimalization using Perf

```sh
perf start <program>

perf record # zaznamenani dat
perf evlist # zobrazeni dat
perf report # zobrazeni vysledku
perf record -g # nahrani nejenom adres preruseni ale cely call graf (tj. i z jake fce bylo co zavolano) - nefunguje dobre
```

## Notes

- nezapomenout prelozit s debugovacima informacema
