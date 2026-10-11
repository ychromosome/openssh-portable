#!/bin/sh
# Assemble the pinned libcrux portable ML-KEM-1024 extraction.
# Only include flattening, whitespace cleanup and internal linkage change.
# The existing ML-KEM-768/ML-DSA extraction is not touched.
set -eu
if [ "$#" -ne 1 ]; then
	echo "usage: $0 /path/to/libcrux" >&2
	exit 1
fi
python3 - "$1" <<'PY'
from pathlib import Path
import re,sys,subprocess,hashlib,json
root=Path(sys.argv[1]).resolve();revision='c46481ce3cd1cc8315e90db114581d8c992c3d7d'
if subprocess.check_output(['git','rev-parse','HEAD'],cwd=root,text=True).strip()!=revision:
 raise SystemExit('libcrux must be at '+revision)
base=root/'libcrux-ml-kem/extracts/c/generated'
files=['karamel/include/krml/internal/target.h','eurydice_glue.h','libcrux_core.h','internal/libcrux_core.h','libcrux_sha3_portable.h','libcrux_mlkem_portable.h','internal/libcrux_mlkem_portable.h','libcrux_mlkem1024_portable.h','internal/libcrux_mlkem1024_portable.h','libcrux_core.c','libcrux_sha3_portable.c','libcrux_mlkem_portable.c','libcrux_mlkem1024_portable.c']
# Reuse precisely the endian/compiler preamble from the established extraction.
old=Path('libcrux_internal.h').read_text();start=old.index('#if !defined(__GNUC__)');end=old.index('/* from libcrux/',start)
result='/* ML-KEM-1024 from libcrux '+revision+'. */\n'+(root/'LICENSE-MIT').read_text().join(['/*\n','\n*/\n'])+old[start:end]
pattern=re.compile(r'(?m)^((?:(?:static|inline|KRML_MUSTINLINE|KRML_NOINLINE)\s+)*)(?:const\s+)?[A-Za-z_]\w*\s+[*\s]*[A-Za-z_]\w*\(')
def local(match):
 prefix=match.group(1)
 if 'static' in prefix.split():
  return match.group(0).replace('static', 'static __attribute__((unused))', 1)
 return 'static __attribute__((unused)) '+match.group(0)

manifest=[]
for name in files:
 raw=subprocess.check_output(['git','show',revision+':libcrux-ml-kem/extracts/c/generated/'+name],cwd=root);text=raw.decode();manifest.append({'file':name,'sha256':hashlib.sha256(raw).hexdigest()})
 text=re.sub(r'^#include.*\n','',text,flags=re.M)
 text=re.sub(r'[\t ]+$','',text,flags=re.M)
 text=re.sub(r' <<\n\s+\(uint32_t\)\(int32_t\)0','',text)
 text=text.replace('union U {','union {')
 text=pattern.sub(local,text)
 result+='\n/* from libcrux/libcrux-ml-kem/extracts/c/generated/'+name+' */\n'+text+'\n'
Path('libcrux_mlkem1024_internal.h').write_text(result)
Path('libcrux_mlkem1024_sources.json').write_text(json.dumps({'revision':revision,'generation':subprocess.check_output(['git','show',revision+':libcrux-ml-kem/extracts/c/generated/code_gen.txt'],cwd=root,text=True),'changes':['remove includes','trim whitespace','remove generated shift-by-zero','anonymous unions','internal static linkage with unused markers; original inline/noinline attributes preserved'],'files':manifest},indent=2)+'\n')
print('libcrux_mlkem1024_internal.h assembled from pinned source files',file=sys.stderr)
PY
