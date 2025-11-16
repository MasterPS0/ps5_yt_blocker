# YT Update Blocker

A simple GUI tool built with PyQt6 to block YouTube app updates on your PS5. This prevents accidental updates that could softlock the app if you connect to the internet unintentionally. The tool modifies the `appinfo.db` file to spoof the version and redirect update checks to a local address.

This project is a GUI wrapper around the core logic from [Gezine's Y2JB repository](https://github.com/Gezine/Y2JB/). Huge thanks to Gezine for the original `appinfo_editor.py` script—without it, this wouldn't exist!

## Features
- User-friendly GUI for selecting database and backup paths.
- Automatically creates a backup of the original `appinfo.db` file.
- Modifies the YouTube entry in `appinfo.db` to block updates:
  - Sets `CONTENT_VERSION` to `99.999.999` (a fake high version).
  - Sets `VERSION_FILE_URI` to `http://127.0.0.2` (a non-existent local URL).
- Real-time logging in the app window for transparency.
- Menu links to social channels for support and updates.
- Lightweight and easy to use—no advanced coding knowledge required.

## Requirements
- Python 3.6+ (tested on 3.12).
- PyQt6 (install via `pip install pyqt6`).
- SQLite3 (included in Python standard library).
- Access to your PS5 via FTP (e.g., using FileZilla or any FTP client).
- The YouTube PKG must already be installed on your PS5.

**Note:** This tool is for educational and personal use only. Modifying system files can risk database corruption—always follow the steps carefully and reboot if needed.

## Installation
1. Clone or download this repository:
   ```
   git clone https://github.com/sinajet/yt-update-blocker.git
   ```
   (Replace with your actual repo URL if different.)

2. Install dependencies:
   ```
   pip install pyqt6
   ```

3. Ensure the app icons (`app_pics` folder) and `icon.ico` are in the same directory as `yt_blocker.py`.

## Usage
### Important Notes
- If you're using a PS5 backup file version 1.2.1 or higher (from official releases), you can skip this process as updates might already be handled.
- Always close the YouTube app, navigate to Settings, and ensure no packages are installing/updating before replacing files via FTP to avoid corruption.
- If you encounter a database corruption notification after replacement, reboot your PS5.

### Step-by-Step Guide
1. **Retrieve the Database File:**
   - After installing the YouTube PKG on your PS5, connect to your PS5 via FTP.
   - Navigate to `/system_data/priv/mms/` and download `appinfo.db` to your computer.

2. **Run the Tool:**
   - Launch the script:
     ```
     python yt_blocker.py
     ```
   - The GUI window will open.

3. **Select Paths in the GUI:**
   - Click "Path..." next to "appinfo.db Path" and select your downloaded `appinfo.db` file.
   - Click "Path..." next to "Backup Path" and select a folder to save the original backup (e.g., your desktop).
   - Click "Start Process" to modify the file. The log window will show progress and any errors.

4. **Verify Changes:**
   - The tool will create a backup named `appinfo.org.db` in your selected backup folder.
   - It updates the YouTube entry (titleId: 'PPSA01650') to block updates.
   - Check the log for confirmation: It should show updated values and verification.

5. **Replace the File on PS5:**
   - Use FTP to upload the modified `appinfo.db` back to `/system_data/priv/mms/` on your PS5.
   - Overwrite the existing file.

6. **Test and Reboot:**
   - If no corruption notification appears, reboot your PS5.
   - Launch YouTube—it should now be protected from updates even if connected to the internet.

### Troubleshooting
- **Error: Paths are empty!** Ensure both paths are selected before starting.
- **Error: appinfo.db file not found!** Verify the file name and path.
- **Database Corruption:** Follow the notes above—close apps, check settings, and reboot.
- If issues persist, check the console/log output for details.

## Credits
- **Core Logic:** Massive shoutout to [Gezine](https://github.com/Gezine) for the original update-blocking script in [Y2JB](https://github.com/Gezine/Y2JB/). This project builds directly on that foundation.
- **GUI Implementation:** Developed by SinaJet as an enhanced, user-friendly version.

## About Us
- [Telegram Channel](https://t.me/sinajet) – For updates and support.
- [YouTube Channel](https://www.youtube.com/c/SinaJet) – Tutorials and more.
- [GitHub Page](https://github.com/sinajet) – Check out my other projects.

If you find this useful, star the repo or drop a message—feedback is appreciated! 🚀
