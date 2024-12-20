# zone2jsonl

Simple streaming converter for zone files to a JSONL format, based on simdzone.
I made this specifically to work with CZDS dumps, and error handling is mostly non-existent.

## Usage

```
nix run github:schlarpc/zone2jsonl -- some-zone-file
```

or streaming from stdin:

```
cat some-zone-file | nix run github:schlarpc/zone2jsonl -- -
```

## Performance

Overall not great at the moment! Running zone-bench from simdzone on the .com zone file
takes about 45 seconds, and running it through this tool takes over 15 minutes.
It's a little silly that we're going through the pipeline of `text -> wire format (simdzone) -> structs (ldns) -> text`,
and the JSON serialization is just using the first library I came up with.

Quick benchmarks show:
* 30-40% of the time is spent in ldns_wire2rdf inside rr_from_rdata
* 50% of the time is spent in JSON serialization, about 25% in object gen and the rest in json_dumps

Memory usage is solid though and handles multi-GB inputs fine, since everything is streaming.
