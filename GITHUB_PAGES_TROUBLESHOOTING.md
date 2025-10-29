# 🔧 GitHub Pages Deployment Troubleshooting

## ❌ Error: "Get Pages site failed. Please verify that the repository has Pages enabled"

This error occurs when GitHub Pages is not enabled in your repository settings. Here are the solutions:

## 🚀 **Solution 1: Enable GitHub Pages Manually (Recommended)**

### Step 1: Repository Settings
1. Go to your GitHub repository: `https://github.com/rakshitbharat/ESP32-Motion-Detection-Telegram-Notifier`
2. Click **"Settings"** tab (at the top of the repository)
3. Scroll down to **"Pages"** in the left sidebar

### Step 2: Configure Pages Source
**Choose ONE of these options:**

#### Option A: GitHub Actions (Recommended)
- **Source**: Select **"GitHub Actions"**
- **Save** (no other settings needed)

#### Option B: Deploy from Branch
- **Source**: Select **"Deploy from a branch"**
- **Branch**: Select **"main"**
- **Folder**: Select **"/ (root)"** or **"docs"**
- **Save**

### Step 3: Verify Deployment
- Your site will be available at: `https://rakshitbharat.github.io/ESP32-Motion-Detection-Telegram-Notifier/`
- It may take 5-10 minutes to go live

## 🛠️ **Solution 2: Manual Deployment Script**

If GitHub Actions continues to fail, use the manual deployment:

```bash
# Run the manual deployment script
./deploy-pages-manual.sh

# Then enable Pages in Settings → Pages → Deploy from branch → main/docs
```

## 🔍 **Solution 3: Alternative GitHub Actions Workflow**

Replace `.github/workflows/deploy-pages.yml` with this simpler version:

```yaml
name: Deploy to GitHub Pages

on:
  push:
    branches: [ main ]
    paths: [ 'docs/**' ]
  workflow_dispatch:

permissions:
  contents: read
  pages: write
  id-token: write

jobs:
  deploy:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      
      - name: Setup Pages
        uses: actions/configure-pages@v4
        
      - name: Upload artifact
        uses: actions/upload-pages-artifact@v3
        with:
          path: docs
          
      - name: Deploy to GitHub Pages
        uses: actions/deploy-pages@v4
```

## 🐛 **Common Issues & Solutions**

### Issue: "Pages not found (404)"
**Solution**: 
- Ensure `docs/telegram-tester.html` exists
- Check repository name matches URLs in documentation
- Wait 5-10 minutes for propagation

### Issue: "Workflow permission denied"
**Solution**:
1. Go to Settings → Actions → General
2. Set "Workflow permissions" to "Read and write permissions"
3. Check "Allow GitHub Actions to create and approve pull requests"
4. Save

### Issue: "Action failed with pages not enabled"
**Solution**:
1. Manually enable Pages first (Solution 1)
2. Re-run the failed workflow
3. Or use manual deployment (Solution 2)

## ✅ **Verification Steps**

After enabling Pages, verify it works:

1. **Check Status**: Go to Actions tab, ensure workflows are green
2. **Test URL**: Visit `https://rakshitbharat.github.io/ESP32-Motion-Detection-Telegram-Notifier/`
3. **Test Tester**: Try the Telegram configuration tester
4. **Check Links**: Ensure all GitHub links work correctly

## 📞 **Quick Fix Commands**

```bash
# Re-run GitHub Actions deployment
git add .
git commit -m "Trigger Pages deployment"
git push origin main

# Or use manual deployment
./deploy-pages-manual.sh
```

## 🎯 **Expected Result**

Once working, you should have:
- ✅ **Main Page**: Repository documentation portal
- ✅ **Telegram Tester**: Interactive configuration validator  
- ✅ **All Links**: Working references between documentation
- ✅ **Professional UI**: Responsive design on all devices

## 🆘 **Still Having Issues?**

If you continue to have problems:

1. **Check Repository Settings**: Ensure Pages is enabled
2. **Verify Permissions**: Actions should have read/write access
3. **Use Manual Method**: Run `./deploy-pages-manual.sh`
4. **Check File Paths**: Ensure docs/telegram-tester.html exists
5. **Wait for Propagation**: GitHub Pages can take 10-20 minutes

---

**💡 Pro Tip**: Start with manual Pages enablement, then GitHub Actions will work automatically!