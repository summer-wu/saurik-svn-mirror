#!/usr/bin/env bash

current=$(date --iso-8601=minutes)

../utils/prefix.sh "$current" | ../utils/uuidify.sh | cat feed.dat - | tail -n 15 > feed.new
mv -f feed.new feed.dat

cat << EOF
<?xml version="1.0"?>
<atom:feed xmlns:atom="http://www.w3.org/2005/Atom">
    <atom:title>Bank of America</atom:title>
    <atom:updated>$current</atom:updated>

    <atom:author>
        <atom:name>Jay Freeman (saurik)</atom:name>
    </atom:author>

    <atom:id>urn:uuid:f1fea92c-0f8f-11da-811f-000423883b9f</atom:id>
EOF

tac feed.dat | while read -r uuid update process amount description; do
    description=${description#\"}
    description=${description%\"}
    description=$(echo "$description" | codify xml-encode)

    cat << EOF

    <atom:entry>
        <atom:title>$process = $amount - $description</atom:title>
        <atom:id>urn:uuid:$uuid</atom:id>
        <atom:updated>$update</atom:updated>
        <atom:content>$process = $amount - $description</atom:content>
    </atom:entry>
EOF
done

cat << EOF
</atom:feed>
EOF
