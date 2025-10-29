#!/bin/bash
# deploy-pages-manual.sh - Manual GitHub Pages deployment script
# Use this if GitHub Actions deployment fails

set -e

echo "🚀 Manual GitHub Pages Deployment"
echo "=================================="

# Check if we're in a git repository
if [ ! -d ".git" ]; then
    echo "❌ Error: Not in a git repository"
    exit 1
fi

# Check if docs folder exists
if [ ! -d "docs" ]; then
    echo "❌ Error: docs folder not found"
    exit 1
fi

# Check if main files exist
if [ ! -f "docs/telegram-tester.html" ]; then
    echo "❌ Error: telegram-tester.html not found in docs/"
    exit 1
fi

echo "✅ Repository validation passed"

# Create index.html as a copy of telegram-tester.html
echo "📄 Creating index.html from telegram-tester.html..."
cp docs/telegram-tester.html docs/index.html

# Update git
echo "📤 Committing changes to git..."
git add docs/
git commit -m "Deploy GitHub Pages - Manual deployment" || echo "No changes to commit"

# Push to main branch
echo "🚀 Pushing to main branch..."
git push origin main

echo ""
echo "✅ Manual deployment complete!"
echo ""
echo "📋 Next steps:"
echo "  1. Go to your GitHub repository settings"
echo "  2. Navigate to Settings → Pages"
echo "  3. Set Source to 'Deploy from a branch'"
echo "  4. Select Branch: main / docs folder"
echo "  5. Save"
echo ""
echo "🌐 Your tester will be available at:"
echo "  https://rakshitbharat.github.io/ESP32-Motion-Detection-Telegram-Notifier/"
echo ""
echo "⏳ GitHub Pages deployment may take a few minutes to go live."

# Clean up
rm -f docs/index.html
echo "🧹 Cleanup completed"