# GitHub Pages Setup Instructions

## 🚀 Quick Setup

1. **Enable GitHub Pages** in your repository settings:
   - Go to **Settings** → **Pages** (in your GitHub repository)
   - **Source**: Select "GitHub Actions"
   - **Save**

2. **Alternative: Manual Branch Setup** (if GitHub Actions option not available):
   - **Source**: Deploy from a branch
   - **Branch**: `main` / `docs` folder
   - **Save**

3. **Update URLs** in documentation:
   Replace `YOUR_USERNAME` with your actual GitHub username in these files:
   - `QUICK_SETUP.md` (line 18, 89)
   - `README.md` (line 47, 198)
   - `setup.sh` (line 18)
   - `TELEGRAM_TESTER.md` (multiple locations)
   - `docs/README.md` (multiple locations)

3. **Access Your Tester**:
   - Your tester will be available at: `https://rakshitbharat.github.io/ESP32-Motion-Detection-Telegram-Notifier/`
   - GitHub Actions will automatically deploy when you push changes to `docs/`

## 🔧 Customization

### Update Repository Links
The HTML tester references:
```html
https://github.com/rakshitbharat/ESP32-Motion-Detection-Telegram-Notifier
```

This is already correctly configured for your repository.

### Domain Configuration
If using a custom domain, update the URLs accordingly in all documentation files.

## 📁 File Structure
```
docs/
├── telegram-tester.html    # Main tester application
├── README.md              # Documentation portal
└── (auto-generated files from GitHub Actions)
```

## 🛠️ GitHub Actions
The workflow `.github/workflows/deploy-pages.yml` automatically:
- Builds the documentation site
- Deploys to GitHub Pages
- Updates on every push to `docs/` folder

## ✅ Verification
After setup, verify:
1. GitHub Pages is enabled and deployed
2. Tester is accessible at your GitHub Pages URL
3. All documentation links work correctly
4. Telegram API calls work from your domain

## 🆘 Troubleshooting
- **404 Error**: Check GitHub Pages settings and branch configuration
- **CORS Issues**: GitHub Pages should work with Telegram API by default
- **Broken Links**: Update all `YOUR_USERNAME` placeholders with actual username