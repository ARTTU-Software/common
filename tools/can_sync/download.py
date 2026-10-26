"""Download an Anyone/Can view SharePoint workbook, keeping redirect cookies."""

import argparse
import hashlib
import io
import os
import zipfile
from http.cookiejar import CookieJar
from pathlib import Path
from urllib.error import HTTPError, URLError
from urllib.parse import parse_qsl, urlencode, urlsplit, urlunsplit
from urllib.request import HTTPCookieProcessor, Request, build_opener


MAX_DOWNLOAD = 10 * 1024 * 1024


def download_url(url):
    parts = urlsplit(url)
    if parts.scheme != "https" or not (parts.hostname or "").endswith(".sharepoint.com"):
        raise ValueError("CAN_WORKBOOK_URL must be an HTTPS SharePoint sharing link")
    query = [(key, value) for key, value in parse_qsl(parts.query) if key.lower() != "download"]
    query.append(("download", "1"))
    return urlunsplit(parts._replace(query=urlencode(query), fragment=""))


def download(url, destination):
    # SharePoint sets guest access cookies on intermediate redirects. A plain
    # urlopen without a cookie jar can incorrectly end up at Microsoft sign-in.
    opener = build_opener(HTTPCookieProcessor(CookieJar()))
    request = Request(download_url(url), headers={"User-Agent": "Mozilla/5.0", "Cache-Control": "no-cache"})
    with opener.open(request, timeout=45) as response:
        content = response.read(MAX_DOWNLOAD + 1)
    if len(content) > MAX_DOWNLOAD:
        raise ValueError("Workbook exceeds the 10 MiB download limit")
    try:
        with zipfile.ZipFile(io.BytesIO(content)) as archive:
            if "xl/workbook.xml" not in archive.namelist():
                raise ValueError("Downloaded ZIP is not an Excel workbook")
            if sum(item.file_size for item in archive.infolist()) > 64 * 1024 * 1024:
                raise ValueError("Expanded workbook exceeds the 64 MiB limit")
    except zipfile.BadZipFile:
        raise ValueError("SharePoint returned a sign-in/error page, not Excel. Use an Anyone/Can view link with downloads allowed.") from None
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_bytes(content)
    return {"workbook_sha256": hashlib.sha256(content).hexdigest()}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    url = os.environ.get("CAN_WORKBOOK_URL", "").strip()
    if not url:
        parser.exit(1, "Missing repository secret CAN_WORKBOOK_URL. See docs/can-database-sync.md.\n")
    try:
        metadata = download(url, args.output)
    except HTTPError as error:
        parser.exit(1, f"SharePoint download failed (HTTP {error.code}); check sharing permissions/link expiry.\n")
    except (ValueError, OSError, URLError) as error:
        # Do not print network exceptions: their URLs may contain the sharing token.
        detail = str(error) if isinstance(error, ValueError) else type(error).__name__
        parser.exit(1, f"SharePoint download failed: {detail}\n")
    print(f"Downloaded Excel workbook ({args.output.stat().st_size} bytes); SHA256 {metadata['workbook_sha256']}")


if __name__ == "__main__":
    main()
