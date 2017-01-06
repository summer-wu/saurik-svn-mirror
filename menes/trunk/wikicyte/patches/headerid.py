Index: library/markdown/extensions/headerid.py
===================================================================
--- library/markdown/extensions/headerid.py	(revision 2880)
+++ library/markdown/extensions/headerid.py	(working copy)
@@ -71,7 +71,7 @@
 from string import ascii_lowercase, digits, punctuation
 
 ID_CHARS = ascii_lowercase + digits + '-_'
-IDCOUNT_RE = re.compile(r'^(.*)_([0-9]+)$')
+IDCOUNT_RE = re.compile(r'^(.*)-([0-9]+)$')
 
 
 class HeaderIdProcessor(markdown.blockprocessors.BlockProcessor):
@@ -146,16 +146,16 @@
         while id in self.IDs:
             m = IDCOUNT_RE.match(id)
             if m:
-                id = '%s_%d'% (m.group(1), int(m.group(2))+1)
+                id = '%s-%d'% (m.group(1), int(m.group(2))+1)
             else:
-                id = '%s_%d'% (id, 1)
+                id = '%s-%d'% (id, 1)
         self.IDs.append(id)
         return id
 
     def _create_id(self, header):
         """ Return ID from Header text. """
         h = ''
-        for c in header.lower().replace(' ', '_'):
+        for c in header.lower().replace(' ', '-'):
             if c in ID_CHARS:
                 h += c
             elif c not in punctuation:
