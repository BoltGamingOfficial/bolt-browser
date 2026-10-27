# Maintainer: BOLT Browser contributors
pkgname=bolt-browser
pkgver=0.2.0
pkgrel=1
pkgdesc='Lightweight Firefox customization assets and opt-in policy helper'
arch=('any')
url='https://github.com/BoltGamingOfficial/bolt-browser'
license=('unknown')
depends=('firefox' 'python')
source=(
  'startpage.html::startpage/index.html'
  'policies.json::firefox/policies.json'
  'enable-firefox-policy.sh::packaging/enable-firefox-policy.sh'
)
sha256sums=('SKIP' 'SKIP' 'SKIP')

package() {
  install -Dm644 "$srcdir/startpage.html" "$pkgdir/usr/share/bolt-browser/startpage.html"
  install -Dm644 "$srcdir/policies.json" "$pkgdir/usr/share/bolt-browser/policies.json.example"
  install -Dm755 "$srcdir/enable-firefox-policy.sh" "$pkgdir/usr/bin/bolt-browser-enable-firefox-policy"
}
