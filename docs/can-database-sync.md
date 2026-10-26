# SharePoint CAN database sync

Edit the CAN workbook in SharePoint as usual. GitHub downloads the latest saved
workbook, validates the definitions, and generates `MAIN_DBC.dbc`,
`drivers/can/can_signal_defs.h`, and `drivers/can/can_board_defs.h` together.
The workbook remains the editable source; generated files should not be edited
by hand.

## Get the DBC immediately

1. Save your changes in SharePoint and wait for Excel to finish saving.
2. Open **ARTTU-Software/common → Actions → Sync CAN database → Run workflow**.
3. Select `main` and run it.
4. When it succeeds, download the **can-database** artifact on the run page.
   It contains the DBC, both headers, and a source/generator hash manifest.

If definitions changed, the workflow tests them, commits directly to `main`, and
creates the next patch tag. For example, `v1.4.2` becomes `v1.4.3`. It fetches
remote tags and increments the highest stable numeric version, ignoring
prereleases. No initial version is invented if no stable version exists.

The tag's release workflow then attaches the same committed DBC and headers to
the GitHub release. The run artifact is available before that separate release
workflow finishes. Unchanged definitions still produce a downloadable artifact,
without an empty commit or version increment. Formatting and CAN Plan changes
alone do not cause a release.

## One-time repository setup

Create a repository Actions secret named **CAN_WORKBOOK_URL**, containing a
SharePoint **Anyone with the link → Can view** sharing link. Downloads must be
allowed. Treat the link as a credential: anyone holding it can read the workbook.
Do not commit it to Git, place it in a repository variable, or print it in logs.
Replacing the secret later changes the source workbook without a code change.

The downloader keeps the guest cookies set during SharePoint redirects and
adds `download=1`. It rejects sign-in pages, invalid workbooks, and oversized
downloads. No Microsoft app registration or browser session is needed for this
sharing mode. Organization-only links require a different authentication setup.

Enable automatic checks by setting the repository Actions variable
**CAN_SYNC_ENABLED** to `true`. Scheduled checks run hourly at minute 17 UTC;
GitHub may delay scheduled runs. Set it to `false` to pause scheduled checks;
the manual button remains available.

The workflow uses the repository's built-in `GITHUB_TOKEN`, with `contents: write`
for commits/tags and `actions: write` to explicitly dispatch the release workflow.
The repository must permit those workflow permissions and direct pushes to
`main`. If branch or tag rules prohibit the push, the sync fails rather than
forcing or bypassing those rules. No personal token or additional GitHub App
secret is required.

Token-generated pushes do not start normal push workflows, so the sync explicitly
dispatches `release.yml` at the new tag. This preserves the existing shared
release workflow and documentation notification using `DOCS_DISPATCH_TOKEN`.
If release creation or asset upload fails after the atomic push succeeds, rerun
the sync. It finds the tag of the last source manifest commit and retries a
missing DBC release attachment without creating another commit/tag.

## Validation and failure behavior

The converter accepts board tabs with `Type`, `Msg Name`, `ID (Hex)`, `DLC`,
`Signal Name`, `Start Bit`, `Length`, `Byte Order`, and `Data Type` headings.
Columns can be reordered. Tabs are discovered from their `BO_`/`SG_` rows, in any
position and under any name. Tabs without `Type` or message/signal rows are
skipped, including diagnostics tables and empty board templates such as PDM.
Board and signal names use spaces as underscores, matching the previous tool.

Message rows use `BO_`; following signal rows use `SG_`. Classic CAN supports
standard IDs `0x000..0x7FF` and extended IDs up to `0x1FFFFFFF`, with DLC `0..8`.
The converter encodes extended IDs using DBC bit 31; the header generator emits
the actual 29-bit ID and an `IS_EXTENDED` macro for those frames. Bit order is
`Intel` or `Motorola`; signal
types are `Signed`, `Unsigned`, or `Bool`. Optional columns include
`Frequency (Hz)`, `Factor`, `Offset`, `Minimum`, `Maximum`, `Unit`, `Receivers`,
`Value Table`, and `Multiplex` (`M`, `m0`, `m1`, or a numeric branch ID).
Numeric frequencies produce cycle times; descriptive event frequencies remain
text. Blank Factor defaults to 1; blank Offset, Minimum, and Maximum default to
0, as in the existing tool. Other malformed values are errors, not defaults.
Use literal values in definition cells; formulas can remain on CAN Plan.

Duplicate messages/signals, invalid identifiers, invalid scaling/value tables,
overlapping signals, and signals outside their message fail validation.
The error identifies the sheet/row where possible. The converter uses cantools
for an independent strict DBC check and the existing `dbc_to_c_builder.py` for
the C definitions. It does not replace the firmware's CAN serialization code.

Generation and Python regression tests must pass. If definitions changed, the
workflow also runs `bundle exec ceedling test:all`; generated headers must compile
as C11. Only then can it publish. Validation/test failures leave remote `main`
and tags unchanged. Check the failed step in Actions for the reason.

Manual and scheduled runs share one concurrency group. The publisher rejects
unrelated checkout changes and a `main` branch that advanced during generation.
It pushes the commit and tag atomically, without force. If another writer wins
a branch/tag race, rerun to generate from the latest `main`.

The manifest records the workbook SHA256, generator hashes, and generated output
hashes. It contains no sharing link. The workflow does not commit the XLSX.
An updated `common` release does not change boards' pinned submodule revisions;
boards adopt the release separately.

## Run locally

```powershell
python -m pip install -r tools/can_sync/requirements.txt
python -m unittest discover -s tools/can_sync/tests -q
python tools/can_sync/pipeline.py prepare --workbook "CAN IDs V3.xlsx" --artifacts build/can-sync/artifacts
```

The prepare command writes only to the selected artifact directory. It does not
commit, tag, push, or overwrite the repository's generated definitions. For
headless XLSX-to-DBC conversion alone:

```powershell
python tools/can_sync/excel_to_dbc.py "CAN IDs V3.xlsx" --output build/can-sync/MAIN_DBC.dbc
```
