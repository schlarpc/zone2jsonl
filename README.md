# zone2jsonl

Simple streaming converter for zone files to a JSONL format, based on simdzone.
I made this specifically to work with CZDS dumps, and error handling is mostly non-existent.

## Usage

```
nix run github:schlarpc/zone2jsonl -- some-zone-file
```

## Performance

Overall not great at the moment! Running zone-bench from simdzone on the .com zone file
takes about 45 seconds, and running it through this tool takes over 15 minutes.
It's a little silly that we're going through the pipeline of `text -> wire format (simdzone) -> structs (ldns) -> text`,
and the JSON serialization is just using the first library I came up with. I suspect this
can be dramatically improved by working on those two aspects, but I haven't benchmarked anything yet.

Memory usage is solid though and handles multi-GB inputs fine, since everything is streaming.
