import sys
if sys.hexversion < 0x03060000:
    sys.exit("Python 3.6 or newer is required to run this program.")

import argparse
import csv
import datetime
from google.cloud import bigquery
import json
import logging
import os
import subprocess

LATEST_STABLE_DATE = datetime.datetime.utcnow().date() - datetime.timedelta(days=3)
INITIAL_RELEASE_DATE = datetime.date(2018, 12, 18)
MODULE_NAME = "azure-cognitiveservices-speech"
BIGQUERY_TIMEOUT_SECONDS = 120

def valid_end_date(s):
    try:
        date = datetime.datetime.strptime(s, '%Y-%m-%d').date()
        if date > LATEST_STABLE_DATE:
            raise Exception(f'date too recent, must be at least three days ago: {s}')
        return date
    except ValueError:
        raise argparse.ArgumentTypeError(f'invalid date: {s}')

def valid_non_negative_int(s):
    intvalue = int(s)
    if intvalue < 0:
        raise argparse.ArgumentTypeError(f'non-negative integer expected, got: {s}')
    return intvalue

parser = argparse.ArgumentParser(description=f'Retrieve and aggregate PyPI downloads counts for {MODULE_NAME}')
parser.add_argument("--end-date",
                    type=valid_end_date,
                    default=LATEST_STABLE_DATE,
                    help="end date - format YYYY-MM-DD, at least three days ago")
parser.add_argument("--look-back-days",
                    type=valid_non_negative_int,
                    default=7,
                    help="days to look back")
parser.add_argument('--download-cooked',
                    action='store_true',
                    help="download cooked data")
parser.add_argument('--upload-cooked',
                    action='store_true',
                    help="upload cooked data")
parser.add_argument('--blob-storage-key',
                    help="Blob Storage Key")
parser.add_argument('--blob-cooked-url',
                    default='https://csspeechstorage.blob.core.windows.net/usagedata/pypi',
                    help="Blob Cooked URL")
parser.add_argument('--cooked-dir',
                    required=True,
                    help="Temporary directory for collecting data")
parser.add_argument('--verbose',
                    action='store_true',
                    help="verbose output")
parser.add_argument('--force',
                    action='store_true',
                    help="force data upload without download")
args = parser.parse_args()

if args.verbose:
    root = logging.getLogger()
    root.setLevel(logging.INFO)

if not os.environ.get('GOOGLE_APPLICATION_CREDENTIALS'):
    raise Exception("GOOGLE_APPLICATION_CREDENTIALS environment variable needs to be set")

azcopy = "c:/Program Files (x86)/Microsoft SDKs/Azure/AzCopy/AzCopy.exe"

needBlobOperation = args.download_cooked or args.upload_cooked

if needBlobOperation and not args.blob_storage_key:
    raise Exception("--blob-storage-key required with download/upload")

if needBlobOperation and not os.path.isfile(azcopy):
    raise FileNotFoundError(azcopy)

if args.upload_cooked and not (args.download_cooked or args.force):
    raise Exception("--download-cooked or --force required for --upload_cooked")

# Normalize cooked_dir
args.cooked_dir = os.path.abspath(args.cooked_dir)

def cooked_path(cooked_dir, date):
    return os.path.join(cooked_dir, 'bydate', date.strftime('%Y-%m'), date.strftime('%Y-%m-%d.csv'))

def ensure_parent_dir_exists(path):
    directory, file = os.path.split(path)
    if not os.path.isdir(directory):
        os.makedirs(directory)

bigquery_client = None
def get_data(cooked_path, date):
    global bigquery_client
    if not bigquery_client:
        bigquery_client = bigquery.Client()

    table_suffix = date.strftime('%Y%m%d')
    cols = ['day', 'timestamp', 'country_code', 'url', 'file', 'details', 'tls_protocol', 'tls_cipher']
    query = f"""
      SELECT _TABLE_SUFFIX AS day, {", ".join(cols[1:])}
      FROM `the-psf.pypi.downloads*`
      WHERE
        _TABLE_SUFFIX = "{table_suffix}" AND
        file.project = "{MODULE_NAME}"
    """
    logging.debug(query)

    ensure_parent_dir_exists(cooked_path)

    query_job = bigquery_client.query(query)
    query_rows = query_job.result(timeout=BIGQUERY_TIMEOUT_SECONDS)

    rows = []
    rows.extend([json.dumps(item, default=str) for item in row] for row in query_rows)

    logging.info(f'{len(rows)} row(s) retrieved for date {date}, {query_job.total_bytes_billed / 1024 / 1024 / 1024} GiB billed; now trying to save')
    # TODO raise an error if query becomes to expensive?

    try:
        with open(cooked_path, 'w', encoding='utf-8') as csvfile:
            csvwriter = csv.writer(csvfile, lineterminator='\n')
            csvwriter.writerow(cols)
            csvwriter.writerows(rows)
    except Exception as ex:
        logging.error(f'Exception caught when writing {cooked_path}, trying to remove partial')
        if os.path.isfile(path):
            os.remove(path)
        raise ex

if args.download_cooked:
    logging.info(f'Downloading cooked data from {args.blob_cooked_url}')
    cmd = [azcopy, '/y', f'/SourceKey:{args.blob_storage_key}', f'/Source:{args.blob_cooked_url}', '/s', f'/Dest:{args.cooked_dir}', '/mt', '/xo']
    result = subprocess.run(cmd, capture_output=True)
    if result.returncode != 0:
        print(result.stderr)
        raise Exception("Could not download cooked data")

logging.info('Ingest missing data within look back window')

current_date = args.end_date - datetime.timedelta(days=args.look_back_days)
if current_date < INITIAL_RELEASE_DATE:
    logging.warning(f'Will not look back past initial release date, {INITIAL_RELEASE_DATE}')
    current_date = INITIAL_RELEASE_DATE

while current_date <= args.end_date:
    path = cooked_path(args.cooked_dir, current_date)
    if os.path.isfile(path):
        logging.info(f'Skipping date {current_date}, already present.')
    else:
        logging.info(f'Ingesting date {current_date}')
        get_data(path, current_date)
    current_date += datetime.timedelta(days=1)

logging.info('Creating download counts by day since initial release')

overall_csv = os.path.join(args.cooked_dir, 'pypibyday.csv')
ensure_parent_dir_exists(overall_csv)
with open(overall_csv, 'w', encoding='utf-8') as csvoutput:
    csvwriter = csv.writer(csvoutput, lineterminator='\n')
    csvwriter.writerow(['day', 'count'])

    current_date = INITIAL_RELEASE_DATE
    while current_date <= LATEST_STABLE_DATE:
        path = cooked_path(args.cooked_dir, current_date)
        if os.path.isfile(path):
            pip_row_count = 0
            nonpip_row_count = 0
            with open(path, 'r', encoding='utf-8') as csvfile:
                skip_first = True
                csvreader = csv.reader(csvfile)
                for row in csvreader:
                    if skip_first: # header
                        skip_first = False
                    else:
                        try:
                            details = json.loads(row[5])
                            assert(details['installer']['name'] == 'pip')
                            pip_row_count += 1
                        except (json.decoder.JSONDecodeError, KeyError, AssertionError, TypeError):
                            nonpip_row_count += 1
                row = [current_date.strftime('%Y-%m-%d'), pip_row_count]
                csvwriter.writerow(row)
                logging.info(f'Date {current_date} - rows pip: {pip_row_count}, rows non-pip: {nonpip_row_count}')
        else:
            logging.warning(f'File for date {current_date} missing')
        current_date += datetime.timedelta(days=1)

if args.upload_cooked:
    logging.info('Uploading cooked data')
    cmd = [azcopy, '/y', f'/DestKey:{args.blob_storage_key}', f'/Source:{args.cooked_dir}', '/s', f'/Dest:{args.blob_cooked_url}', '/xo']
    result = subprocess.run(cmd, capture_output=True)
    if result.returncode != 0:
        print(result.stderr)
        raise Exception("Could not upload cooked data")
