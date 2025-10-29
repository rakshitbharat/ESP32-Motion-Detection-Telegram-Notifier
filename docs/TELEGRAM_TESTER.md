# 🧪 Telegram Configuration Tester

## Overview

The Telegram Configuration Tester is a web-based tool that validates your Telegram bot setup before uploading code to your ESP32. It tests the exact same Bot Token and Chat ID values that your ESP32 will use.

## 🔗 Access the Tester

**Live Tool:** [https://rakshitbharat.github.io/ESP32-Motion-Detection-Telegram-Notifier/](https://rakshitbharat.github.io/ESP32-Motion-Detection-Telegram-Notifier/)

## ✅ Why Use the Tester?

1. **Pre-validate Configuration**: Catch Telegram setup issues before uploading to ESP32
2. **Save Development Time**: No need to upload/debug on hardware for Telegram issues
3. **Exact Value Testing**: Tests the same Bot Token and Chat ID used in `secrets.h`
4. **Real Message Testing**: Sends actual test messages to verify end-to-end functionality
5. **Visual Validation**: Clear success/error feedback with detailed troubleshooting

## 🎯 What the Tester Validates

### Bot Token Validation
- ✅ Format correctness (contains `:` and proper length)
- ✅ API accessibility (can communicate with Telegram servers)
- ✅ Bot existence and permissions

### Chat ID Validation  
- ✅ Format correctness (numeric, proper sign for groups)
- ✅ Chat accessibility (bot can send messages)
- ✅ Message delivery (actual test message sent)

### Connection Testing
- ✅ Network connectivity to Telegram API
- ✅ API response validation
- ✅ Error message interpretation

## 🔄 Integration with ESP32 Project

### Value Mapping
The tester uses the exact same values as your ESP32 code:

| Tester Field | ESP32 Code (`secrets.h`) | Description |
|-------------|-------------------------|-------------|
| Bot Token | `const char* BOT_TOKEN` | Token from @BotFather |
| Chat ID | `const char* CHAT_ID` | Your chat identifier |

### Workflow Integration
1. **Create Bot** → Get Token from @BotFather
2. **Choose Chat Type** → Personal, Group, or Channel setup
3. **Setup Chat** → Add bot as member/admin (for groups/channels)
4. **Get Chat ID** → Interactive finder with guided setup
5. **Test Online** → Use web tester to validate everything
6. **Copy Values** → Update `secrets.h` with tested values
7. **Upload ESP32** → Deploy with confidence

## 🛠️ Technical Details

### Implementation
- **Pure HTML/CSS/JavaScript** - No server-side dependencies
- **Direct Telegram API Calls** - Tests real API endpoints
- **GitHub Pages Hosted** - Automatically deployed via GitHub Actions
- **CORS Compatible** - Works with Telegram's API CORS policy

### API Endpoints Tested
1. `GET /bot<token>/getMe` - Validates bot token and retrieves bot info
2. `POST /bot<token>/sendMessage` - Tests message sending capability

### Security Considerations
- **Client-Side Only** - No credentials stored or transmitted to third parties
- **Direct API Communication** - Goes directly from your browser to Telegram
- **No Logging** - No data persistence or analytics tracking

## 🔧 Troubleshooting Common Issues

### Bot Token Errors
```
Error: Bot Token Error: Unauthorized
```
**Solution:** Double-check token from @BotFather, ensure no extra spaces

### Chat ID Errors  
```
Error: Send Message Error: Chat not found
```
**Solutions:**
- Ensure you sent a message to the bot first
- For groups: Add bot as admin
- Verify Chat ID format (negative for groups)

### Network Errors
```
Error: Failed to fetch
```
**Solutions:**
- Check internet connection
- Try different browser
- Disable ad blockers temporarily

## 🚀 Deployment (For Contributors)

The tester is automatically deployed via GitHub Actions when changes are pushed to the `docs/` folder:

```yaml
# .github/workflows/deploy-pages.yml
name: Deploy Telegram Tester to GitHub Pages
on:
  push:
    branches: [ main ]
    paths: [ 'docs/**' ]
```

### Local Development
```bash
# Serve locally for testing
python -m http.server 8000
# Open: http://localhost:8000/docs/telegram-tester.html
```

## 📊 Usage Analytics

The tester provides real-time feedback:
- ✅ **Format Validation** - Immediate feedback on input format
- 🔄 **Live Testing** - Real API calls with loading indicators  
- 📝 **Detailed Results** - Success confirmations with bot info
- 🛠️ **Error Guidance** - Specific troubleshooting for each error type

## 🎯 Future Enhancements

Planned improvements:
- [ ] Multiple chat ID testing
- [ ] Bot command testing (`/start`, `/help`)
- [ ] Configuration export (generate `secrets.h` snippet)
- [ ] Advanced message formatting testing
- [ ] Group/channel specific validations

## 💡 Tips for Best Results

1. **Test in Private Chat First** - Easier to get Chat ID
2. **Keep Bot Token Secure** - Don't share or commit tokens
3. **Test Before Each Deployment** - Validate after any bot changes
4. **Use Group Admin Rights** - For group/channel notifications
5. **Double-Check Copy/Paste** - Ensure no extra characters in values

---

**🔗 Quick Links:**
- [Live Tester](https://rakshitbharat.github.io/ESP32-Motion-Detection-Telegram-Notifier/)
- [Quick Setup Guide](QUICK_SETUP.md)
- [Main Documentation](README.md)
- [Advanced Configuration](ADVANCED_CONFIG.md)