# Pagine importanti dello standard
- 28-29:    fasi di traduzione
- 70-71:    ordini di converione tra tipi
- 78:       __func__

# Rank di conversion
- long long
- long
- int
- short
- unsigned char
- _Bool

- se stessa segnatura, convesione alla precisione più alta
- se uno è unsigned di rank maggiore o uguale all'altro signed, conversione a quello unsigned 
- se uno è signed di rank maggiore a quello unsigned, conversione a quello signed
- altrimenti, convertiti all'unsigned con rank del valore signed
