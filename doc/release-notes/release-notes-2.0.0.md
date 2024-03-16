How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes in some cases), then run the
installer (on Windows) or just copy over `/Applications/Lyncoin-Qt` (on macOS)
or `lyncoind`/`lyncoin-qt` (on Linux).

Notable changes
===============

Wallet
------
- The `createwallet` RPC will no longer create legacy (BDB) wallets when
  setting `descriptors=false` without also providing the
  `-deprecatedrpc=create_bdb` option. This is because the legacy wallet is
  being deprecated in a future release. ([#28597](https://github.com/bitcoin/bitcoin/pull/28597))

- The `gettransaction`, `listtransactions`, `listsinceblock` RPCs now return
  the `abandoned` field for all transactions. Previously, the "abandoned" field
  was only returned for sent transactions. ([#25158](https://github.com/bitcoin/bitcoin/pull/25158))

- The `getbalances` RPC now returns a `lastprocessedblock` JSON object which contains the wallet's last processed block
  hash and height at the time the balances were calculated. This result shouldn't be cached because importing new keys could invalidate it. ([#26094](https://github.com/bitcoin/bitcoin/pull/26094))

- The `gettransaction` RPC now returns a `lastprocessedblock` JSON object which contains the wallet's last processed block
  hash and height at the time the transaction information was generated. ([#26094](https://github.com/bitcoin/bitcoin/pull/26094))

- The `getwalletinfo` RPC now returns a `lastprocessedblock` JSON object which contains the wallet's last processed block
  hash and height at the time the wallet information was generated. ([#26094](https://github.com/bitcoin/bitcoin/pull/26094))

- Coin selection and transaction building now accounts for unconfirmed low-feerate ancestor transactions. When it is necessary to spend unconfirmed outputs, the wallet will add fees to ensure that the new transaction with its ancestors will achieve a mining score equal to the feerate requested by the user. ([#26152](https://github.com/bitcoin/bitcoin/pull/26152))

Descriptors
-----------

- The usage of hybrid public keys in output descriptors has been removed. Hybrid
  public keys are an exotic public key encoding not supported by output descriptors
  (as specified in [BIP380](https://github.com/bitcoin/bips/blob/master/bip-0380.mediawiki) and documented in doc/descriptors.md). Lyncoin Core would
  previously incorrectly accept descriptors containing such hybrid keys. ([#28587](https://github.com/bitcoin/bitcoin/pull/28587))

GUI changes
-----------

- The transaction list in the GUI no longer provides a special category for "payment to yourself". Now transactions that have both inputs and outputs that affect the wallet are displayed on separate lines for spending and receiving. (gui#119)

- A new menu option allows migrating a legacy wallet based on keys and implied output script types stored in BerkeleyDB (BDB) to a modern wallet that uses descriptors stored in SQLite. ([gui#738](https://github.com/bitcoin-core/gui/pull/738))

- The PSBT operations dialog marks outputs paying your own wallet with "own address". ([gui#740](https://github.com/bitcoin-core/gui/pull/740))

- The ability to create legacy wallets is being removed. ([gui#764](https://github.com/bitcoin-core/gui/pull/764))


### P2P

- [#27626](https://github.com/bitcoin/bitcoin/pull/27626) Parallel compact block downloads
- [#27743](https://github.com/bitcoin/bitcoin/pull/27743) p2p: Unconditionally return when compact block status == READ_STATUS_FAILED
- [#28038](https://github.com/bitcoin/bitcoin/pull/28038) wallet: address book migration bug fixes
- [#28067](https://github.com/bitcoin/bitcoin/pull/28067) descriptors: do not return top-level only funcs as sub descriptors
- [#28125](https://github.com/bitcoin/bitcoin/pull/28125) wallet: bugfix, disallow migration of invalid scripts
- [#28542](https://github.com/bitcoin/bitcoin/pull/28542) wallet: Check for uninitialized last processed and conflicting heights in MarkConflicted

### Wallet

- [#28994](https://github.com/bitcoin/bitcoin/pull/28994) wallet: skip BnB when SFFO is enabled
- [#28920](https://github.com/bitcoin/bitcoin/pull/28920) wallet: birth time update during tx scanning
- [#29176](https://github.com/bitcoin/bitcoin/pull/29176) wallet: Fix use-after-free in WalletBatch::EraseRecords

### Gui

- [gui#751](https://github.com/bitcoin-core/gui/pull/751) macOS, do not process actions during shutdown

### Fees

- [#27622](https://github.com/bitcoin/bitcoin/pull/27622) Fee estimation: avoid serving stale fee estimate

### RPC

- [#29003](https://github.com/bitcoin/bitcoin/pull/29003) rpc: fix getrawtransaction segfault
- [#27727](https://github.com/bitcoin/bitcoin/pull/27727) rpc: Fix invalid bech32 address handling

### Rest

- [#28551](https://github.com/bitcoin/bitcoin/pull/28551) http: bugfix: allow server shutdown in case of remote client disconnection

### Logs

- [#29227](https://github.com/bitcoin/bitcoin/pull/29227) log mempool loading progress

### P2P and network changes

- [#29200](https://github.com/bitcoin/bitcoin/pull/29200) net: create I2P sessions using both ECIES-X25519 and ElGamal encryption

### Build

- [#29127](https://github.com/bitcoin/bitcoin/pull/29127) Use hardened runtime on macOS release builds.
- [#29195](https://github.com/bitcoin/bitcoin/pull/29195) build: Fix -Xclang -internal-isystem option
- [#27724](https://github.com/bitcoin/bitcoin/pull/27724) build: disable boost multi index safe mode in debug mode
- [#28097](https://github.com/bitcoin/bitcoin/pull/28097) depends: xcb-proto 1.15.2
- [#28543](https://github.com/bitcoin/bitcoin/pull/28543) build, macos: Fix qt package build with new Xcode 15 linker
- [#28571](https://github.com/bitcoin/bitcoin/pull/28571) depends: fix unusable memory_resource in macos qt build

### Miscellaneous

- [#28391](https://github.com/bitcoin/bitcoin/pull/28391) refactor: Simplify CTxMempool/BlockAssembler fields, remove some external mapTx access
- [#28791](https://github.com/bitcoin/bitcoin/pull/28791) snapshots: don't core dump when running -checkblockindex after loadtxoutset
- [#28452](https://github.com/bitcoin/bitcoin/pull/28452) Do not use std::vector = {} to release memory