How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes in some cases), then run the
installer (on Windows) or just copy over `/Applications/Lyncoin-Qt` (on macOS)
or `lyncoind`/`lyncoin-qt` (on Linux).

Notable changes
===============

Proof-of-Work algorithm
-------------------------
- The Proof-of-Work algorithm will change from sha256d (bitcoin)
  to the Flex algorithm after block height 260500

mempool.dat compatibility
-------------------------

- The `mempool.dat` file created by -persistmempool or the savemempool RPC will
  be written in a new format. This new format includes the XOR'ing of transaction
  contents to mitigate issues where external programs (such as anti-virus) attempt
  to interpret and potentially modify the file.

  This new format can not be read by previous software releases. To allow for a
  downgrade, a temporary setting `-persistmempoolv1` has been added to fall back
  to the legacy format. ([#28207](https://github.com/bitcoin/bitcoin/pull/28207))

P2P and network changes
-----------------------

- BIP324 v2 transport is now enabled by default. It remains possible to disable v2
  by running with `-v2transport=0`. ([#29347](https://github.com/bitcoin/bitcoin/pull/29347))
- Manual connection options (`-connect`, `-addnode` and `-seednode`) will
  now follow `-v2transport` to connect with v2 by default. They will retry with
  v1 on failure. ([#29058](https://github.com/bitcoin/bitcoin/pull/29058))

- Network-adjusted time has been removed from consensus code. It is replaced
  with (unadjusted) system time. The warning for a large median time offset
  (70 minutes or more) is kept. This removes the implicit security assumption of
  requiring an honest majority of outbound peers, and increases the importance
  of the node operator ensuring their system time is (and stays) correct to not
  fall out of consensus with the network. ([#28956](https://github.com/bitcoin/bitcoin/pull/28956))

External Signing
----------------

- Support for external signing on Windows has been disabled. It will be re-enabled
  once the underlying dependency (Boost Process), has been replaced with a different
  library. ([#28967](https://github.com/bitcoin/bitcoin/pull/28967))

Updated RPCs
------------

- The addnode RPC now follows the `-v2transport` option (now on by default, see above) for making connections.
  It remains possible to specify the transport type manually with the v2transport argument of addnode. ([#29239](https://github.com/bitcoin/bitcoin/pull/29239))

Build System
------------

- A C++20 capable compiler is now required to build Lyncoin Core. ([#28349](https://github.com/bitcoin/bitcoin/pull/28349))
- MacOS releases are configured to use the hardened runtime libraries ([#29127](https://github.com/bitcoin/bitcoin/pull/29127))

Wallet
------

- The CoinGrinder coin selection algorithm has been introduced to mitigate unnecessary
  large input sets and lower transaction costs at high feerates. CoinGrinder
  searches for the input set with minimal weight. Solutions found by
  CoinGrinder will produce a change output. CoinGrinder is only active at
  elevated feerates (default: 30+ sat/vB, based on `-consolidatefeerate`×3). ([#27877](https://github.com/bitcoin/bitcoin/pull/27877))
- The Branch And Bound coin selection algorithm will be disabled when the subtract fee
  from outputs feature is used. ([#28994](https://github.com/bitcoin/bitcoin/pull/28994))
- If the birth time of a descriptor is detected to be later than the first transaction
  involving that descriptor, the birth time will be reset to the earlier time. ([#28920](https://github.com/bitcoin/bitcoin/pull/28920))

Low-level changes
=================

Pruning
-------

- When pruning during initial block download, more blocks will be pruned at each
  flush in order to speed up the syncing of such nodes. ([#20827](https://github.com/bitcoin/bitcoin/pull/20827))

Init
----

- Various fixes to prevent issues where subsequent instances of Lyncoin Core would
  result in deletion of files in use by an existing instance. ([#28784](https://github.com/bitcoin/bitcoin/pull/28784), [#28946](https://github.com/bitcoin/bitcoin/pull/28946))
- Improved handling of empty `settings.json` files. ([#29144](https://github.com/bitcoin/bitcoin/pull/29144))