(declare (unit precsi))
(declare (uses example))

;; display prelude to csi
(display "zlib\n\n")

(display "  A SWIG example for the CHICKEN compiler\n")
(display "  Author: Jonah Beckford, February 2003\n\n")

(display "Scheme Procedures:\n")
(display "
zlib-max-mem-level
zlib-max-wbits
zlib-seek-set
zlib-seek-cur
zlib-seek-end
zlib-version
zlib-z-stream-next-in-set
zlib-z-stream-next-in-get
zlib-z-stream-avail-in-set
zlib-z-stream-avail-in-get
zlib-z-stream-total-in-set
zlib-z-stream-total-in-get
zlib-z-stream-next-out-set
zlib-z-stream-next-out-get
zlib-z-stream-avail-out-set
zlib-z-stream-avail-out-get
zlib-z-stream-total-out-set
zlib-z-stream-total-out-get
zlib-z-stream-msg-set
zlib-z-stream-msg-get
zlib-z-stream-state-set
zlib-z-stream-state-get
zlib-z-stream-zalloc-set
zlib-z-stream-zalloc-get
zlib-z-stream-zfree-set
zlib-z-stream-zfree-get
zlib-z-stream-opaque-set
zlib-z-stream-opaque-get
zlib-z-stream-data-type-set
zlib-z-stream-data-type-get
zlib-z-stream-adler-set
zlib-z-stream-adler-get
zlib-z-stream-reserved-set
zlib-z-stream-reserved-get
zlib-new-z-stream
zlib-delete-z-stream
zlib-z-no-flush
zlib-z-partial-flush
zlib-z-sync-flush
zlib-z-full-flush
zlib-z-finish
zlib-z-ok
zlib-z-stream-end
zlib-z-need-dict
zlib-z-errno
zlib-z-stream-error
zlib-z-data-error
zlib-z-mem-error
zlib-z-buf-error
zlib-z-version-error
zlib-z-no-compression
zlib-z-best-speed
zlib-z-best-compression
zlib-z-default-compression
zlib-z-filtered
zlib-z-huffman-only
zlib-z-default-strategy
zlib-z-binary
zlib-z-ascii
zlib-z-unknown
zlib-z-deflated
zlib-z-null
zlib-version
zlib-deflate
zlib-deflate-end
zlib-inflate
zlib-inflate-end
zlib-deflate-set-dictionary
zlib-deflate-copy
zlib-deflate-reset
zlib-deflate-params
zlib-inflate-set-dictionary
zlib-inflate-sync
zlib-inflate-reset
zlib-compress
zlib-compress2
zlib-uncompress
zlib-gzopen
zlib-gzdopen
zlib-gzsetparams
zlib-gzread
zlib-gzwrite
zlib-gzprintf
zlib-gzputs
zlib-gzgets
zlib-gzputc
zlib-gzgetc
zlib-gzflush
zlib-gzseek
zlib-gzrewind
zlib-gztell
zlib-gzeof
zlib-gzclose
zlib-gzerror
zlib-adler32
zlib-crc32
zlib-deflate-init-
zlib-inflate-init-
zlib-deflate-init2-
zlib-inflate-init2-
zlib-internal-state-dummy-set
zlib-internal-state-dummy-get
zlib-new-internal-state
zlib-delete-internal-state
zlib-z-error
zlib-inflate-sync-point
zlib-get-crc-table
zlib-deflate-init
zlib-inflate-init
zlib-z-stream-save-next-out
zlib-z-stream-get-next-chunk
")